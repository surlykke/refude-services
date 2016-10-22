#include <ripc/utils.h>
#include <ripc/jsonwriter.h>
#include "xdg.h"
#include "desktopentryreader.h"
#include "mimeappslistreader.h"
#include "applicationcollector.h"

namespace org_restfulipc
{
    ApplicationCollector::ApplicationCollector() : 
        defaultApplications(),
        jsonArray(JsonConst::EmptyArray)
    {
    }

    ApplicationCollector::~ApplicationCollector()
    {
    }

    void ApplicationCollector::collect()
    {
        Map<Json> jsonMap;
        for (const std::string& applicationsDir : append(xdg::data_dirs(), "/applications")) {
            readDesktopFiles(directoryTree(applicationsDir), jsonMap);
            readMimeappsListFile(applicationsDir, jsonMap);
        }

        readDesktopFiles(directoryTree(xdg::data_home() + "/applications"), jsonMap);

        for (std::string configDir : xdg::config_dirs()) {
            readMimeappsListFile(configDir, jsonMap);
        }

        readMimeappsListFile(xdg::config_home(), jsonMap);

        jsonMap.each([this](const char* key, Json& json) {
            jsonArray.append(std::move(json));
        });
    }

   
    void ApplicationCollector::readDesktopFiles(std::vector<std::string> applicationsDirs, Map<Json>& jsonMap)
    {
        for (const std::string& applicationsDir : applicationsDirs) {
            for (const std::string& desktopFile : files(applicationsDir, {".desktop"})) {
                std::string desktopFilePath = applicationsDir + desktopFile; 
                std::string appId = desktopFile;
                std::replace(appId.begin(), appId.end(), '/', '-');
                std::string icon = appId.substr(0, appId.size() - 8); // strip ending '.desktop'
                
                DesktopEntryReader reader(desktopFilePath);
                if (reader.json.contains("Hidden") && (bool)reader.json["Hidden"]) {
                    jsonMap.erase(appId.data());
                }
                else {
                    if (!reader.json.contains("Icon")) {
                        // Remove '/applications/' at start and  '.desktop' at end
                        reader.json["Icon"] = appId.substr(strlen("/applications/"), appId.size() - 8); 
                    }
                    reader.json["applicationId"] = appId;
                    jsonMap[appId] = std::move(reader.json);
                }
            }
        }
    }

    void ApplicationCollector::readMimeappsListFile(std::string dir, Map<Json>& jsonMap)
    {
        MimeappsList mimeappsList(dir + "/mimeapps.list");
        mimeappsList.removedAssociations.each([&jsonMap](const char* mimetype, 
                                                     std::set<std::string>& deAssociatedApplications) {
            for (const std::string& deAssociatedApplication : deAssociatedApplications) {
                if (jsonMap.contains(deAssociatedApplication)) {
                    Json& associatedAppsArray = jsonMap[deAssociatedApplication]["mimetype"];
                    while (int index = associatedAppsArray.find(deAssociatedApplication) > -1) {
                        associatedAppsArray.take(index);
                    }
                }
            }
        });
        
        mimeappsList.addedAssociations.each([&jsonMap](const char* mimetype, 
                                                   std::set<std::string>& associatedApplications) {
            for (const std::string& associatedApplication : associatedApplications) {
                if (jsonMap.contains(associatedApplication)) {
                    jsonMap[associatedApplication]["mimetype"].append(mimetype);
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