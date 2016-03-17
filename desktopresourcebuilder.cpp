#include <dirent.h>
#include <locale>
#include "errorhandling.h"
#include "jsonresource.h"
#include "utils.h"
#include "desktopentryreader.h"
#include "desktopresourcebuilder.h"
#include "handlerTemplate.h"
#include "jsonreader.h"

namespace org_restfulipc 
{

    DesktopResourceBuilder::DesktopResourceBuilder(Service* service, MimeappsListCollector& mimeappsListCollector) : 
        mimeappsListCollector(mimeappsListCollector),
        service(service),
        processedEntries(),
        fileHandlers(),
        urlHandlers()
    {
        fileHandlers << handlerTemplate_json;
        fileHandlers["_links"]["self"]["href"] = "/desktopentry/filehandlers";

        urlHandlers << handlerTemplate_json;
        urlHandlers["_links"]["self"]["href"] = "/desktopentry/urlhandlers";

    }

    DesktopResourceBuilder::~DesktopResourceBuilder()
    {
    }

    void DesktopResourceBuilder::build()
    {
        findDirs(); 
      
        for (string configDir : configDirs) {
            mimeappsListCollector.collect(MimeappsList(configDir + "/" + "mimeapps.list"));
        }

        for (string applicationsDir : applicationsDirs) {
            mimeappsListCollector.collect(MimeappsList(applicationsDir + "/" + "mimeapps.list"));

            build(applicationsDir, "");
        }

        JsonResource* fileHandlersResource = new JsonResource();
        fileHandlersResource->setJson(std::move(fileHandlers));
        service->map("/desktopentry/filehandlers", fileHandlersResource);

        JsonResource* urlHandlersResource = new JsonResource();
        urlHandlersResource->setJson(std::move(urlHandlers));
        service->map("/desktopentry/urlhandlers", urlHandlersResource);
    }
    
    void DesktopResourceBuilder::findDirs()
    {
        string xdg_config_home = value("XDG_CONFIG_HOME");
        if (xdg_config_home.empty()) xdg_config_home = value("HOME") + "/.config";
        configDirs = { xdg_config_home };

        vector<string> xdg_config_dirs = split(value("XDG_CONFIG_DIRS"), ':');
        if (xdg_config_dirs.empty()) xdg_config_dirs = { "/etc/xdg" };

        for (string xdg_config_dir : xdg_config_dirs) {
            configDirs.push_back(xdg_config_dir);
        }

        string xdg_data_home = value("XDG_DATA_HOME");
        if (xdg_data_home.empty()) xdg_data_home = value("HOME") + "/.local/share";

        applicationsDirs = { xdg_data_home + "/applications" };

        vector<string> xdg_data_dirs = split(value("XDG_DATA_DIRS"), ':');
        if (xdg_data_dirs.empty()) xdg_data_dirs = { "/usr/local/share", "/usr/share" };

        for (string xdg_data_dir : xdg_data_dirs) {
            if (xdg_data_dir != xdg_data_home) { // LXQt (and others?) include XDG_DATA_HOME in XDG_DATA_DIRS
                applicationsDirs.push_back(xdg_data_dir + "/applications");
            }
        }
    
        
    }


    void DesktopResourceBuilder::findDesktopEntriesAndSubdirs(string dirPath, vector<string>& entries, vector<string>& subdirs)
    {
        DIR* dir = opendir(dirPath.data());
        if (dir == NULL) throw C_Error();

        struct dirent dent;
        struct dirent* result;
        for (;;) {
            if (readdir_r(dir, &dent, &result) < 0) {
                throw C_Error();
            }
            else if (result == NULL) {
                return;
            } 
            else if (result->d_name[0] == '.') {
                // "We skip directories ".", ".." and hidden directories/files 
                continue;
            }
            else if (result->d_type == DT_REG || result->d_type == DT_LNK) {
                if (strlen(result->d_name) > 8 && !strcmp(".desktop", result->d_name + strlen(result->d_name) - 8)) {
                    entries.push_back(result->d_name);        
                }
            }
            else if (result->d_type == DT_DIR) {
                subdirs.push_back(result->d_name);
            }
        }
        closedir(dir);
    }


    void DesktopResourceBuilder::build(std::string applicationsDir, std::string dir)
    {
        vector<string> desktopEntries;
        vector<string> subdirs;
        string thisdir = applicationsDir + "/" + dir;

        findDesktopEntriesAndSubdirs(applicationsDir + "/" + dir, desktopEntries, subdirs);
        
        for (string desktopEntry : desktopEntries) {
            string relativePath = dir.empty() ? desktopEntry : dir + "/" + desktopEntry;
            string entryId = relativePath;
            replace(entryId.begin(), entryId.end(), '/', '-') ;
            
            if (processedEntries.count(entryId) >= 1) { 
                continue;
            }

            DesktopEntryReader desktopEntryReader(applicationsDir, relativePath);
            processedEntries.insert(entryId);
            Json& json = desktopEntryReader.json;
            if (! (json.contains("Hidden") && (bool)json["Hidden"])) {
                if (json.contains("MimeType")) {
                    Json& mimetypes = json["MimeType"];
                    for (int i = 0; i < mimetypes.size(); i++) {
                        mimeappsListCollector.addAssociation((const char*)mimetypes[i], entryId);
                    }
                }               
              
                if (json.contains("Exec")) {
                    if (strcasestr(json["Exec"], "%u")) {
                        fileHandlers["handlers"].append(entryId);
                        urlHandlers["handlers"].append(entryId);
                    }
                    else if (strcasestr(json["Exec"], "%f")) {
                        fileHandlers["handlers"].append(entryId);
                    }
                } 

                LocalizedJsonResource* resource = new LocalizedJsonResource();
                string selfRef = string("/desktopentry/") + entryId;
                desktopEntryReader.json["_links"]["self"]["href"] = selfRef; 
                service->map(selfRef.data(), resource);
                resource->setJson(std::move(desktopEntryReader.json), std::move(desktopEntryReader.translations));
            }

        }

        for (string subdir : subdirs) {
            string relativeDir = dir.empty() ? subdir : dir + "/" + subdir;
            build(applicationsDir, relativeDir);
        }
    }

}