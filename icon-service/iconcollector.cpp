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
#include <ripc/json.h>
#include "iconcollector.h"

namespace org_restfulipc
{

    IconCollector::IconCollector(std::string directoryPath, Json& iconDirJson) :
        directoryPath(directoryPath),
        iconDirJson(iconDirJson)
    {
        dir = opendir(directoryPath.data());
    }

    IconCollector::~IconCollector()
    {
        if (dir) {
            closedir(dir);
        }
    }

    void IconCollector::collectInto(IconMap& iconMap)
    {
        if (dir == NULL) {
            return;
        }

        for (;;) {
            std::string fileName;
            std::string filePath;

            errno = 0;
            struct dirent* dirent = readdir(dir);
            if (errno && !dirent) {
                throw C_Error();
            }
            else if (!dirent) {
                break;
            }
            else if (dirent->d_type == DT_REG || dirent->d_type == DT_LNK) {
                std::string fileName = dirent->d_name;
                if (fileName.size() <= 4) {
                    continue;
                }
                std::string fileEnding = fileName.substr(fileName.size() - 4, 4); // All endings '.png', '.xpm', and '.svg' 
                std::string iconName = fileName.substr(0, fileName.size() - 4);   // have same length.
                std::string filePath = directoryPath + '/' + fileName;
                if (dirent->d_type == DT_LNK) {
                    if (!realpath(filePath.data(), buffer)) {
                        std::cerr << "Unable to resolve " << filePath << " - " << strerror(errno) << "\n";
                    }
                    filePath = buffer;
                }

                Json instance = JsonConst::EmptyObject;
                if (fileEnding == ".png") {
                    instance["mimetype"] = "image/png";
                }
                else if (fileEnding == ".xpm") {
                    instance["mimetype"] = "image/x-xpixmap";
                }
                else if (fileEnding == ".svg") {
                    instance["mimetype"] = "image/svg+xml";
                }
                else {
                    continue;
                }
 
                if (iconDirJson.contains("Context")) {
                    instance["context"] = (const char*)iconDirJson["Context"];
                }
                instance["path"] = filePath;
                instance["MinSize"] = (double)iconDirJson["MinSize"];
                instance["MaxSize"] = (double)iconDirJson["MaxSize"];

                if (iconMap[iconName].undefined()) {
                    iconMap[iconName] = JsonConst::EmptyArray;
                }
                iconMap[iconName].append(std::move(instance));
            }
        }
    }
}