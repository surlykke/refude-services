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
    std::vector<std::string> splitByWhitespace(std::string str);
    std::vector<std::string> split(std::string str, char c);
    std::set<std::string> splitToSet(std::string str, char c);
    std::string value(const std::string& envVarName, const std::string& fallback = "");
    std::string replaceAll(const std::string oldString, char ch, char replaceCh);

    std::vector<std::string> append(std::vector<std::string> dirs, std::string subdir);

    
    std::vector<std::string> subdirectories(std::string directory);
    std::vector<std::string> files(const std::string& directory, 
                                   const std::vector<std::string>& fileEndings,
                                   bool includeLinks = true);
    
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
     * Hidden directories are omitted (name starting with '.')
     * 
     * @param directory The directory to start from 
     * @return vector containing the directory tree 'linearlized'     
     */
     std::vector<std::string> directoryTree(std::string directory);

}
#endif /* UTILS_H */

