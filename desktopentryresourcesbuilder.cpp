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

    void DesktopEntryResourceBuilder::buildJsons()
    {
        for (const string& applicationsDir : append(xdg::data_dirs(), "/applications")) {
            readDesktopFiles(directoryTree(applicationsDir));
            readMimeappsListFile(applicationsDir);
        }

        readDesktopFiles(directoryTree(xdg::data_home() + "/applications"));

        for (string configDir : xdg::config_dirs()) {
            readMimeappsListFile(configDir);
        }

        readMimeappsListFile(xdg::config_home());
    }

    void DesktopEntryResourceBuilder::mapResources(Service& service, NotifierResource::ptr notifier)
    {
        if (service.mapping("/desktopentries", true)) {
            DesktopEntryResource::ptr desktopEntryResource = 
                dynamic_pointer_cast<DesktopEntryResource>(service.mapping("/desktopentries", true));
            desktopEntryResource->setDesktopJsons(move(desktopJsons), notifier);
        }
        else {
            DesktopEntryResource::ptr desktopEntryResource = make_shared<DesktopEntryResource>(move(desktopJsons));
            service.map("/desktopentries", desktopEntryResource, true);
        }
    }

    vector<string> DesktopEntryResourceBuilder::desktopFiles(string directory)
    {
        vector<string> files;
        DIR* dir = opendir(directory.data());
        if (dir == NULL) throw C_Error();
        
        for (;;) {
            errno = 0;
            struct dirent* dirent = readdir(dir);
            if (errno && !dirent) { 
                throw C_Error();
            }
            else if (!dirent) {
                break;
            } 
            else if ( (dirent->d_type == DT_REG || dirent->d_type == DT_LNK) && 
                        (strlen(dirent->d_name) > 8 && 
                        !strcmp(".desktop", dirent->d_name + strlen(dirent->d_name) - 8))) {
                files.push_back(directory + dirent->d_name);        
            }
        }
       
        closedir(dir);
        return files;
    } 
    
    void DesktopEntryResourceBuilder::readDesktopFiles(vector<string> applicationsDirs)
    {
        for (const string& applicationsDir : applicationsDirs) {
            for (const string& desktopFilePath : desktopFiles(applicationsDir)) {
                DesktopEntryReader reader(desktopFilePath);
                string entryId = replaceAll(string(desktopFilePath.data() + applicationsDirs[0].size()), '/', '-');
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

    void DesktopEntryResourceBuilder::readMimeappsListFile(string dir)
    {
        MimeappsList mimeappsList(dir + "/mimeapps.list");
        mimeappsList.removedAssociations.each([this](const char* mimetype, set<string>& deAssociatedApplications) {
            for (const string& deAssociatedApplication : deAssociatedApplications) {
                if (desktopJsons.contains(deAssociatedApplication)) {
                    Json& associatedAppsArray = desktopJsons[deAssociatedApplication]["MimeType"];
                    while (int index = associatedAppsArray.find(deAssociatedApplication) > -1) {
                        associatedAppsArray.take(index);
                    }
                }
            }
        });
        
        mimeappsList.addedAssociations.each([this](const char* mimetype, set<string>& associatedApplications) {
            for (const string& associatedApplication : associatedApplications) {
                if (desktopJsons.contains(associatedApplication)) {
                    desktopJsons[associatedApplication]["MimeType"].append(mimetype);
                }
            }
        });

        mimeappsList.defaultApps.each([this](const char* mimetype, vector<string>& defaultApplicationIds){
            for (string& defaultApplicationId : defaultApplicationIds) {
                vector<string>& defaults = defaultApplications[mimetype];
                defaults.erase(std::remove(defaults.begin(), defaults.end(), defaultApplicationId), defaults.end());
            }
            
            int pos = 0;
            for (string& defaultApplicationId : defaultApplicationIds) {
                defaultApplications[mimetype].insert(defaultApplications[mimetype].begin() + pos++, defaultApplicationId);
            }
        });
        
    }

}
