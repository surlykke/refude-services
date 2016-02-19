#include <dirent.h>
#include <locale>
#include "errorhandling.h"
#include "jsonresource.h"
#include "utils.h"
#include "desktopentryreader.h"
#include "desktopresourcebuilder.h"

namespace org_restfulipc 
{

    DesktopResourceBuilder::DesktopResourceBuilder(Service* service, MimeappsListReader& mimeappsListReader) : 
        service(service),
        mimeappsListReader(mimeappsListReader)
    {
    }

    DesktopResourceBuilder::~DesktopResourceBuilder()
    {
    }

    void DesktopResourceBuilder::build()
    {
        findDirs(); 
      
        for (string configDir : configDirs) {
            for (string desktopEnvName : desktopEnvNames) {
                mimeappsListReader.read(configDir + "/" + desktopEnvName + "-" + "mimeapps.list");
            }

            mimeappsListReader.read(configDir + "/" + "mimeapps.list");
        }

        for (string applicationsDir : applicationsDirs) {
            for (string desktopEnvName : desktopEnvNames) {
                mimeappsListReader.read(applicationsDir + "/" + desktopEnvName + "-" + "mimeapps.list");
            }
            mimeappsListReader.read(applicationsDir + "/" + "mimeapps.list");

            build(applicationsDir, "");
        }
    }

    void DesktopResourceBuilder::findDirs()
    {
        vector<string> DE_NAMES = split(value("XDG_CURRENT_DESKTOP"), ':');
        for (string de_name : DE_NAMES) {
            transform(de_name.begin(), de_name.end(), de_name.begin(), ::tolower);
            desktopEnvNames.push_back(de_name);
        }

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
            DesktopEntryReader desktopEntryReader(applicationsDir, relativePath);
            if (!service->mapping(desktopEntryReader.json["_links"]["self"]["href"])) {
                if (desktopEntryReader.json.contains("MimeType")) {
                    Json& mimetypes = desktopEntryReader.json["MimeType"];
                    for (int i = 0; i < mimetypes.size(); i++) {
                        mimeappsListReader.addAssociation(desktopEntryReader.entryId, (const char*)mimetypes[i]);
                    }
                }
                JsonResource* jsonResource = new JsonResource();
                jsonResource->json = std::move(desktopEntryReader.json);
                jsonResource->setResponseStale();
                service->map(jsonResource->json["_links"]["self"]["href"], jsonResource);
            }
        }

        for (string subdir : subdirs) {
            string relativeDir = dir.empty() ? subdir : dir + "/" + subdir;
            build(applicationsDir, relativeDir);
        }
    }
}