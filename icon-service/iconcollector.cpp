/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <sys/stat.h>

#include <iostream>
#include "errorhandling.h"
#include "utils.h"
#include <unistd.h>
#include "json.h"
#include "iconcollector.h"

namespace refude
{

    IconCollector::IconCollector(std::string directoryPath, Json& iconDirJson) :
        directoryPath(directoryPath),
        iconDirJson(iconDirJson)
    {
    }

    IconCollector::~IconCollector()
    {
    }

    void IconCollector::collectInto(IconMap& iconMap)
    {
        for (const std::string& fileName : files(directoryPath, {".png", ".xpm", ".svg"})) {
            std::string filePath = directoryPath + '/' + fileName;
            if (!realpath(filePath.data(), buffer)) {
                std::cerr << "Unable to resolve " << filePath << " - " << strerror(errno) << "\n";
                continue;
            }
            filePath = buffer;

            Json instance = JsonConst::EmptyObject;
            std::string fileEnding = fileName.substr(fileName.size() - 4, 4); // All endings '.png', '.xpm', and '.svg' 
            std::string iconName = fileName.substr(0, fileName.size() - 4);   // have same length.
            
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
                instance["context"] = iconDirJson["Context"].toString();
            }
            instance["path"] = filePath;
            instance["MinSize"] = iconDirJson["MinSize"].toDouble();
            instance["MaxSize"] = iconDirJson["MaxSize"].toDouble();

            if (iconMap[iconName].undefined()) {
                iconMap[iconName] = JsonConst::EmptyArray;
            }
            iconMap[iconName].append(std::move(instance));
        }
        
    }
}
