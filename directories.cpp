/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <dirent.h>
#include <ripc/errorhandling.h>
#include <ripc/utils.h>

#include "directories.h"

namespace org_restfulipc
{

    Directories::Directories()
    {
        string home = value("HOME");
        usersConfigDir = value("XDG_CONFIG_HOME", home + "/.config");
        systemConfigDirs = split(value("XDG_CONFIG_DIRS", "/etc/xdg"), ':');
        string xdg_data_home = value("XDG_DATA_HOME", home + "/.local/share");
        usersApplicationDirRoot = xdg_data_home + "/applications";
        for(const string& xdg_data_dir : split(value("XDG_DATA_DIRS", "/usr/share:/usr/local/share"), ':')) { 
            if (xdg_data_dir != xdg_data_home) {
                systemApplicationDirRoots.push_back(xdg_data_dir + "/applications");
            }
        }
    }

    Directories::~Directories()
    {
    }

    vector<string> Directories::directoryTree(string directory)
    {
        vector<string> directories = { directory + "/" };
        int index = 0; 
        while (index < directories.size()) {
            DIR* dir = opendir(directories[index].data());
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
                else if (dirent->d_name[0] == '.') {
                    // "We skip directories ".", ".." and hidden directories
                    continue;
                }
                else if (dirent->d_type == DT_DIR) {
                    directories.push_back(directories[index] + string(dirent->d_name) + '/');
                }
            }
            closedir(dir);
            index++;
        }
        return directories;
    } 
} 