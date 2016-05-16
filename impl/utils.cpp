/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Restful Inter Process Communication (Ripc) project. 
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#include <dirent.h>
#include <string.h>
#include <algorithm>
#include "errorhandling.h"
#include "utils.h"

namespace org_restfulipc
{

    std::vector<std::string> split(std::string str, char c)
    {
        std::vector<std::string> result;
        int k = 0;
        for (int i = 0; i < str.size(); i++) {
            if (str[i] == c) {
                if (i > k) {
                    result.push_back(str.substr(k, i - k));
                }
                k = i + 1;
            }
        }

        if (k < str.size()) {
            result.push_back(str.substr(k));
        }
        return result;
    }

    std::set<std::string> splitToSet(std::string str, char c)
    {
        std::vector<std::string> vals = split(str, c);
        std::set<std::string> result;
        result.insert(vals.begin(), vals.end());
        return result;
    }

    std::string value(const std::string& envVarName, const std::string& fallback)
    {
        const char* val = getenv(envVarName.data());
        return (val && *val) ? val : fallback;
    }

    std::string replaceAll(const std::string oldString, char ch, char replaceCh)
    {
        std::string result = oldString;
        std::replace(result.begin(), result.end(), ch, replaceCh);
        return result;
    }

    std::vector<std::string> append(std::vector<std::string> dirs, std::string subdir)
    {
        std::vector<std::string> appended;
        for (std::string dir : dirs) {
            appended.push_back(dir + subdir);
        }

        return appended;
    }

    std::vector<std::string> subdirectories(std::string directory)
    {
        std::vector<std::string> result;
        DIR* dir = opendir(directory.data());
        if (dir == NULL) throw C_Error(directory.data());

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
                // "We skip ".", ".." and hidden directories
                continue;
            }
            else if (dirent->d_type == DT_DIR) {
                result.push_back(dirent->d_name);
            }
        }
        closedir(dir);
        return result;
    }

    std::vector<std::string> directoryTree(std::string directory)
    {
        std::vector<std::string> directories = {directory + "/"};
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
                    // "We skip ".", ".." and hidden directories
                    continue;
                }
                else if (dirent->d_type == DT_DIR) {
                    directories.push_back(directories[index] + std::string(dirent->d_name) + '/');
                }
            }
            closedir(dir);
            index++;
        }
        return directories;
    }
}