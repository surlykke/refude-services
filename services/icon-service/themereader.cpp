/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#include <dirent.h>
#include <ripc/utils.h>
#include <ripc/jsonwriter.h>

#include "themereader.h"

namespace org_restfulipc
{

    ThemeReader::ThemeReader(Json& theme, const std::string& dirPath) :
        IniReader(dirPath + "/index.theme"),
        themeJson(theme),
        dirPath(dirPath)
    {
        read();
    }

    ThemeReader::~ThemeReader()
    {
    }

    void ThemeReader::read()
    {
        std::set<std::string> declaredDirectories;
        if (lineType == EndOfFile) {
            return;
        }

        getNextLine();
        if (lineType != Heading || heading != "Icon Theme") {
            throw RuntimeError("'[Icon Theme]' expected");
        }
        while (getNextLine() == KeyValue) {
            if (key == "Name") {
                themeJson["_ripc:localized:Name"][locale] = value;
            }
            else if (key == "Comment") {
                themeJson["_ripc:localized:Comment"][locale] = value;
            }
            else if (key == "Directories") {
                declaredDirectories = splitToSet(value, ',');
            }
            else {
                themeJson[key] = value;
            }
        }
        while (lineType == Heading) {
            if (declaredDirectories.count(heading) > 0) {
                int minSize;
                int maxSize;
                std::string sizeType = "Threshold";
                int size = -1;
                int threshold = 2;
                std::string context;

                while (getNextLine() == KeyValue) {
                    if (key == "Size") {
                        size = stoi(value);
                    }
                    else if (key == "Type") {
                        if (oneOf(value,{"Fixed", "Scalable", "Threshold"})) {
                            sizeType = value;
                        }
                        else {
                            throw RuntimeError("Illegal 'Type' value: %s\n", value);
                        }
                    }
                    else if (key == "Context") {
                        context = value;
                    }
                    else if (key == "MaxSize") {
                        maxSize = stoi(value);
                    }
                    else if (key == "MinSize") {
                        minSize = stoi(value);
                    }
                    else if (key == "Threshold") {
                        threshold = stoi(value);
                    }
                }

                if (size == -1) {
                    throw RuntimeError("No size given");
                }

                if (sizeType == "Fixed") {
                    minSize = maxSize = size;
                }
                else if (sizeType == "Scalable" && (minSize == -1 || maxSize == -1)) {
                    throw RuntimeError("With type 'Scalable' both 'MinSize' and 'MaxSize' must be given");
                }
                else if (sizeType == "Threshold") {
                    minSize = size - threshold;
                    maxSize = size + threshold;
                }
               
                Json& directoryObj = themeJson["IconDirectories"][heading];
                if (directoryObj.undefined()) {
                    directoryObj = JsonConst::EmptyObject;
                }
                if (!context.empty()) {
                    directoryObj["Context"] = context;
                }
                directoryObj["Size"] = size;
                directoryObj["MinSize"] = minSize;
                directoryObj["MaxSize"] = maxSize;
            }
            else {
                throw RuntimeError("Directory group '%' not listed in 'Directories'");
            }
        }
    }

    bool ThemeReader::oneOf(std::string str, std::list<std::string> list, bool caseSensitive)
    {
        if (caseSensitive) {
            for (std::string val : list) if (str == val) return true;
        }
        else {
            for (std::string val :list) if (strcasecmp(str.data(), val.data()) == 0) return true;
        }
        return false;
    }
}
