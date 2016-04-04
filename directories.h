/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef DIRECTORIES_H
#define DIRECTORIES_H
#include <vector>
#include <string>

namespace org_restfulipc
{
    using namespace std;

    struct Directories
    {
        Directories();
        virtual ~Directories();

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
        * @param directory The directory to start from 
        * @return      
        */
        vector<string> directoryTree(string directory);

        string usersConfigDir;   // eg. ~/.config
        vector<string> systemConfigDirs; // eg. /etc/xdg
        string usersApplicationDirRoot;  // eg. ~/.local/share/applications
        vector<string> systemApplicationDirRoots; // eg. /usr
    };
}
#endif /* DIRECTORIES_H */

