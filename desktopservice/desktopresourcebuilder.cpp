#include <dirent.h>
#include "errorhandling.h"
#include "jsonresource.h"
#include "desktopentryreader.h"
#include "desktopresourcebuilder.h"

namespace org_restfulipc 
{

    DesktopResourceBuilder::DesktopResourceBuilder(Service* service) : 
        service(service)
    {
    }

    DesktopResourceBuilder::~DesktopResourceBuilder()
    {
    }

    void DesktopResourceBuilder::build()
    {
        if (std::getenv("XDG_DATA_DIRS")) {
            std::string xdg_data_dirs(std::getenv("XDG_DATA_DIRS"));
            std::size_t remaining = 0;
            std::size_t colonPos = -1;
            while ((colonPos = xdg_data_dirs.find(":", remaining)) != std::string::npos) {
                applicationsDirs.push_back(xdg_data_dirs.substr(remaining, colonPos - remaining) + 
                                           "/applications");
                remaining = colonPos + 1;
            }
            if (remaining < xdg_data_dirs.size()) {
                applicationsDirs.push_back(xdg_data_dirs.substr(remaining) + 
                                           "/applications");
            }
        } 
        else {
            if (std::getenv("HOME")) {
                std::string home(std::getenv("HOME"));
                applicationsDirs.push_back(std::string(std::getenv("HOME")) + "/.local/share/applications");
            }
            applicationsDirs.push_back("/usr/local/share/applications");
            applicationsDirs.push_back("/usr/share/applications");
        }

        for (std::string applicationsDir : applicationsDirs) {
            build(applicationsDir, "");
        }

    }

    void DesktopResourceBuilder::build(std::string applicationsDir, std::string subDir)
    {
        DIR* dir = opendir((applicationsDir + (subDir.empty() ? "" : "/") + subDir).data());
        if (dir == NULL) throw C_Error();

        struct dirent dent;
        struct dirent* result;
        for (;;) {
            if (readdir_r(dir, &dent, &result) < 0) throw C_Error();

            if (result == NULL) {
                return;
            } else if (result->d_name[0] == '.') {
                // "We skip directories ".", ".." and hidden directories/files 
                continue;
            }

            std::string relativePath = subDir.empty() ? result->d_name : subDir + "/" + result->d_name;
            if (result->d_type == DT_REG || result->d_type == DT_LNK) {
                if (strlen(result->d_name) > 8 && !strcmp(".desktop", result->d_name + strlen(result->d_name) - 8)) {
                    DesktopEntryReader desktopEntryReader(applicationsDir, relativePath);
                    if (!service->mapping(desktopEntryReader.json["_links"]["self"]["href"])) {
                        JsonResource* jsonResource = new JsonResource();
                        jsonResource->json = std::move(desktopEntryReader.json);
                        service->map(jsonResource->json["_links"]["self"]["href"], jsonResource);
                    }
                } else {
                }
            } else if (result->d_type == DT_DIR) {
                build(applicationsDir, relativePath);
            }
        }
        closedir(dir);
    }


}