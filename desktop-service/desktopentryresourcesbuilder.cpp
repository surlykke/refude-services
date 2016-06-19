/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <errno.h>
#include <dirent.h>
#include <algorithm>
#include <experimental/filesystem>
#include <ripc/utils.h>
#include <ripc/notifierresource.h>
#include <ripc/jsonwriter.h>

#include "desktopentryreader.h"
#include "mimeappslistreader.h"
#include "handlerTemplate.h"

#include "desktopentryresourcesbuilder.h"
#include "desktopentryresource.h"

namespace org_restfulipc
{
    DesktopEntryResourceBuilder::DesktopEntryResourceBuilder() : 
        defaultApplications(),
        desktopJsons()
    {
    }

    DesktopEntryResourceBuilder::~DesktopEntryResourceBuilder()
    {
    }

    void DesktopEntryResourceBuilder::build()
    {
        for (const std::string& applicationsDir : append(xdg::data_dirs(), "/applications")) {
            readDesktopFiles(directoryTree(applicationsDir));
            readMimeappsListFile(applicationsDir);
        }

        readDesktopFiles(directoryTree(xdg::data_home() + "/applications"));

        for (std::string configDir : xdg::config_dirs()) {
            readMimeappsListFile(configDir);
        }

        readMimeappsListFile(xdg::config_home());
    }

   
    void DesktopEntryResourceBuilder::readDesktopFiles(std::vector<std::string> applicationsDirs)
    {
        for (const std::string& applicationsDir : applicationsDirs) {
            std::cout << "Look for desktopfiles in " << applicationsDir << "\n";
            for (const std::string& desktopFile : files(applicationsDir, {"desktop"})) {
                std::string desktopFilePath = applicationsDir + "/" + desktopFile;
                DesktopEntryReader reader(desktopFilePath);
            
                std::string entryId = replaceAll(std::string(desktopFilePath.data() + applicationsDirs[0].size() + 1), '/', '-');
                std::cout << "adding entryId: "  << entryId << "\n";
                if (reader.json.contains("Hidden") && (bool)reader.json["Hidden"]) {
                    desktopJsons.take(entryId);
                }
                else {
                    if (!reader.json.contains("Icon")) {
                        reader.json["Icon"] = entryId.substr(0, entryId.size() - 8); // Remove '.desktop'
                    }
                    desktopJsons[entryId] = std::move(reader.json);
                }
            }
        }
    }

    void DesktopEntryResourceBuilder::readMimeappsListFile(std::string dir)
    {
        MimeappsList mimeappsList(dir + "/mimeapps.list");
        mimeappsList.removedAssociations.each([this](const char* mimetype, 
                                                     std::set<std::string>& deAssociatedApplications) {
            for (const std::string& deAssociatedApplication : deAssociatedApplications) {
                if (desktopJsons.contains(deAssociatedApplication)) {
                    Json& associatedAppsArray = desktopJsons[deAssociatedApplication]["MimeType"];
                    while (int index = associatedAppsArray.find(deAssociatedApplication) > -1) {
                        associatedAppsArray.take(index);
                    }
                }
            }
        });
        
        mimeappsList.addedAssociations.each([this](const char* mimetype, 
                                                   std::set<std::string>& associatedApplications) {
            for (const std::string& associatedApplication : associatedApplications) {
                if (desktopJsons.contains(associatedApplication)) {
                    desktopJsons[associatedApplication]["MimeType"].append(mimetype);
                }
            }
        });

        mimeappsList.defaultApps.each([this](const char* mimetype, 
                                             std::vector<std::string>& defaultApplicationIds){
            for (std::string& defaultApplicationId : defaultApplicationIds) {
                std::vector<std::string>& defaults = defaultApplications[mimetype];
                defaults.erase(std::remove(defaults.begin(), defaults.end(), defaultApplicationId), defaults.end());
            }
            
            int pos = 0;
            for (std::string& defaultApplicationId : defaultApplicationIds) {
                defaultApplications[mimetype].insert(defaultApplications[mimetype].begin() + pos++, defaultApplicationId);
            }
        });
        
    }

}
