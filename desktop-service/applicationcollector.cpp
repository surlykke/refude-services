/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include "utils.h"
#include "jsonwriter.h"
#include "xdg.h"
#include "desktopentryreader.h"
#include "mimeappslistreader.h"
#include "applicationcollector.h"

namespace refude
{
    ApplicationCollector::ApplicationCollector() : 
        collectedApplications(),
        defaultApplications()
    {
    }

    ApplicationCollector::~ApplicationCollector()
    {
    }

    void ApplicationCollector::collect()
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

   
    void ApplicationCollector::readDesktopFiles(std::vector<std::string> applicationsDirs)
    {
        for (const std::string& applicationsDir : applicationsDirs) {
            for (const std::string& desktopFile : files(applicationsDir, {".desktop"})) {
                std::string desktopFilePath = applicationsDir + desktopFile; 
                std::string appId = desktopFile;
                std::replace(appId.begin(), appId.end(), '/', '-');
                
                DesktopEntryReader reader(desktopFilePath);
                if (reader.json.contains("Hidden") && reader.json["Hidden"].toBool()) {
                    collectedApplications.erase(appId.data());
                }
                else {
                    if (!reader.json.contains("Icon")) {
                        // Remove '/applications/' at start and  '.desktop' at end
                        reader.json["Icon"] = appId.substr(appId.size() - 8); 
                    }
                    reader.json["applicationId"] = appId;
                    collectedApplications[appId] = std::move(reader.json);
                }
            }
        }
    }

    void ApplicationCollector::readMimeappsListFile(std::string dir)
    {
        MimeappsList mimeappsList(dir + "/mimeapps.list");
        for (const auto& entry : mimeappsList.removedAssociations) {
            const std::set<std::string>& deAssociatedApplications = entry.value;
            for (const std::string& deAssociatedApplication : deAssociatedApplications) {
                int pos = collectedApplications.find(deAssociatedApplication);
                if ( pos >= 0) {
                    Json& associatedAppsArray = collectedApplications.pairAt(pos).value["mimetype"];
                    while (int index = associatedAppsArray.find(deAssociatedApplication) > -1) {
                        associatedAppsArray.take(index);
                    }
                }
            }
        };
        
        for (const auto& entry : mimeappsList.addedAssociations) {
            const std::string& mimetype = entry.key;
            const std::set<std::string>& associatedApplications = entry.value;
            for (const std::string& associatedApplication : associatedApplications) {
                int pos = collectedApplications.find(associatedApplication);
                if (pos >= 0) {
                    collectedApplications.pairAt(pos).value["mimetype"].append(mimetype);
                }
            }
        };

        for (const auto& entry : mimeappsList.defaultApps) {
            const std::string& mimetype = entry.key;
            const std::vector<std::string>& defaultApplicationIds = entry.value;
            for (const std::string& defaultApplicationId : defaultApplicationIds) {
                std::vector<std::string>& defaults = defaultApplications[mimetype];
                defaults.erase(std::remove(defaults.begin(), defaults.end(), defaultApplicationId), defaults.end());
            }
            
            int pos = 0;
            for (const std::string& defaultApplicationId : defaultApplicationIds) {
                defaultApplications[mimetype].insert(defaultApplications[mimetype].begin() + pos++, defaultApplicationId);
            }
        }
        
    }

}
