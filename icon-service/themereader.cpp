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

    ThemeReader::ThemeReader(Json& theme, const string& dirPath) :
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
        set<string> declaredDirectories;
        if (lineType == EndOfFile) {
            return;
        }

        getNextLine();
        if (lineType != Heading || heading != "Icon Theme") {
            throw RuntimeError("'[Icon Theme]' expected");
        }
        while (getNextLine() == KeyValue) {
            if (oneOf(key,{"Name", "Comment"})) {
                themeJson[key][locale] = value;
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
                string sizeType = "Threshold";
                int size = -1;
                int threshold = 2;
                string context;

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

    //            IconCollector(dirPath + '/' + heading, minSize, maxSize, context).collectInto(iconMap);
            }
            else {
                throw RuntimeError("Directory group '%' not listed in 'Directories'");
            }
        }
    }

    bool ThemeReader::oneOf(string str, std::list<std::string> list)
    {
        for (std::string val : list) if (str == val) return true;
        return false;
    }
}
