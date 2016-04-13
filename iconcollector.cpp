/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <dirent.h>
#include <iostream>
#include <ripc/errorhandling.h>
#include "iconcollector.h"

namespace org_restfulipc
{

    IconCollector::IconCollector(string directoryPath)
    {
        dir = opendir(directoryPath.data());
        if (dir == NULL) {
            return;
        }
        
        for (;;) {
            errno = 0;
            struct dirent* dirent = readdir(dir);
            if (errno && !dirent) { 
                throw C_Error();
            }
            else if (!dirent) {
                break;
            } 
            else if ( dirent->d_type == DT_REG) {
                string fileName = dirent->d_name;
                if (fileName.size() <= 4) {
                    continue;
                }

                string fileEnding = fileName.substr(fileName.size() - 4, 4);
                string iconName = fileName.substr(0, fileName.size() - 4); // All endings '.png', '.xpm', and '.svg' 
                                                                           // have same length.
                IconInstance& instance = collectedIcons[iconName];
                if (fileEnding == ".png") {
                    instance.mimetype = "image/png";
                }
                else if (fileEnding == ".xpm") {
                    instance.mimetype = "image/x-xpixmap";
                }
                else if (fileEnding == ".svg") {
                    instance.mimetype = "image/svg+xml";
                }
                else {
                    continue;
                }
               instance.path = directoryPath + "/" + dirent->d_name;
                instance.maxSize = 0;
                instance.minSize = 0;
            }
        }
    }


    IconCollector::~IconCollector()
    {
        if (dir) {
            closedir(dir);
        }
    }

}