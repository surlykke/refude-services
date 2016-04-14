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

    vector<string> split(string str, char c)
    {
        std::vector<string> result;
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

    set<string> splitToSet(string str, char c)
    {
        vector<string> vals = split(str, c);
        set<string> result;
        result.insert(vals.begin(), vals.end());
        return result;
    }

    string value(const string& envVarName, const string& fallback)
    {
        const char* val = getenv(envVarName.data());
        return (val && *val) ? val : fallback;
    }

    string replaceAll(const string oldString, char ch, char replaceCh)
    {
        string result = oldString;
        replace(result.begin(), result.end(), ch, replaceCh);
        return result;
    }

    vector<string> append(vector<string> dirs, string subdir)
    {
        vector<string> appended;
        for (string dir : dirs) {
            appended.push_back(dir + subdir);
        }

        return appended;
    }

    vector<string> subdirectories(string directory)
    {
        vector<string> result;
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

    vector<string> directoryTree(string directory)
    {
        vector<string> directories = {directory + "/"};
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
                    directories.push_back(directories[index] + string(dirent->d_name) + '/');
                }
            }
            closedir(dir);
            index++;
        }
        return directories;
    }
}