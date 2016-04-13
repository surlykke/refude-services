/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Restful Inter Process Communication (Ripc) project. 
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <set>
#include <string>

namespace org_restfulipc
{
    using namespace std;

    vector<string> split(string str, char c);
    set<string> splitToSet(string str, char c);
    string value(const string& envVarName, const string& fallback = "");
    string replaceAll(const string oldString, char ch, char replaceCh);

    vector<string> append(vector<string> dirs, string subdir);

    /**
     * Given a directory produces a vector containing, as first, the starting directory, and, after that,
     * all its sub- subsub- etc- directories. All directorypaths end with '/'.
     * Example: Given the directory /usr/share/applications, and assuming /usr/share/applications contains 
     * directories xxx and yyy/zzz, the returned vector may look like:
     * 
     * { "/usr/share/applications/", 
     *   "/usr/share/applications/xxx/", 
     *   "/usr/share/applications/yyy/", 
     *   "/usr/share/applications/yyy/zzz/"}
     *
     * Handy when you need to traverse a directory 
     *
     * @param directory The directory to start from 
     * @return vector containing the directory tree 'linearlized'     
     */
     vector<string> directoryTree(string directory);

}
#endif /* UTILS_H */

