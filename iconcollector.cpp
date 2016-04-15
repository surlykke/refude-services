/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <dirent.h>
#include <sys/stat.h>

#include <iostream>
#include <ripc/errorhandling.h>
#include <unistd.h>
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
            string fileName;
            string filePath;

            errno = 0;
            struct dirent* dirent = readdir(dir);
            if (errno && !dirent) {
                throw C_Error();
            }
            else if (!dirent) {
                break;
            }
            else if (dirent->d_type == DT_REG || dirent->d_type == DT_LNK) {
                string fileName = dirent->d_name;
                if (fileName.size() <= 4) {
                    continue;
                }
                string fileEnding = fileName.substr(fileName.size() - 4, 4); // All endings '.png', '.xpm', and '.svg' 
                string iconName = fileName.substr(0, fileName.size() - 4); // have same length.
                string filePath = directoryPath + '/' + fileName;
                if (dirent->d_type == DT_LNK) {
                    if (!realpath(filePath.data(), buffer)) {
                        std::cerr << "Unable to resolve " << filePath << " - " << strerror(errno) << "\n";
                    }
                    filePath = buffer;
                }

                IconInstance instance;
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

                if (fileName.size() <= 4) {
                    continue;
                }

                instance.path = filePath;
                instance.maxSize = 0;
                instance.minSize = 0;

                collectedIcons[iconName] = move(instance);
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