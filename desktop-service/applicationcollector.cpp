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
        applicationJsons(JsonConst::EmptyObject),
        filehandlerJsons(JsonConst::EmptyObject),
        urlhandlerJsons(JsonConst::EmptyObject)
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
            for (const std::string& desktopFile : files(applicationsDir, {"desktop"})) {
                std::string desktopFilePath = applicationsDir + desktopFile; 
                std::string key = desktopFile;
                std::replace(key.begin(), key.end(), '/', '-');
                std::string icon = key.substr(0, key.size() - 8); // strip ending '.desktop'
                
                DesktopEntryReader reader(desktopFilePath);
                if (reader.json.contains("Hidden") && (bool)reader.json["Hidden"]) {
                    applicationJsons.erase(key.data());
                    filehandlerJsons.erase(key.data());
                    urlhandlerJsons.erase(key.data()); 
                }
                else {
                    if (!reader.json.contains("Icon")) {
                        // Remove '/applications/' at start and  '.desktop' at end
                        reader.json["Icon"] = key.substr(strlen("/applications/"), key.size() - 8); 
                    }
                     
                    applicationJsons[key] = std::move(reader.json);
                }
            }
        }
    }

    void ApplicationCollector::readMimeappsListFile(std::string dir)
    {
        MimeappsList mimeappsList(dir + "/mimeapps.list");
        mimeappsList.removedAssociations.each([this](const char* mimetype, 
                                                     std::set<std::string>& deAssociatedApplications) {
            for (const std::string& deAssociatedApplication : deAssociatedApplications) {
                if (applicationJsons.contains(deAssociatedApplication)) {
                    Json& associatedAppsArray = applicationJsons[deAssociatedApplication]["MimeType"];
                    while (int index = associatedAppsArray.find(deAssociatedApplication) > -1) {
                        associatedAppsArray.take(index);
                    }
                }
            }
        });
        
        mimeappsList.addedAssociations.each([this](const char* mimetype, 
                                                   std::set<std::string>& associatedApplications) {
            for (const std::string& associatedApplication : associatedApplications) {
                if (applicationJsons.contains(associatedApplication)) {
                    applicationJsons[associatedApplication]["MimeType"].append(mimetype);
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