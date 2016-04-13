/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <ripc/utils.h>
#include <ripc/jsonwriter.h>
#include "indexthemereader.h"
namespace org_restfulipc
{

    IndexThemeReader::IndexThemeReader(string indexThemeFilePath) : 
        IniReader(indexThemeFilePath),
        json(JsonConst::EmptyObject),
        translations(JsonConst::EmptyObject)
    {
        
    }

    IndexThemeReader::~IndexThemeReader()
    {
    }

    void IndexThemeReader::read()
    {
        getNextLine(); 
        if (lineType != Heading || heading != "Icon Theme") {
            throw RuntimeError("'[Icon Theme]' expected");
        }
        while (getNextLine() == KeyValue) {
            readKeyValue(json);
        }
        while (lineType == Heading) {
            if (declaredDirectories.count(heading) > 0) {
                iconDirectories.push_back(readDirectoryGroup());
            }
            else {
                throw RuntimeError("Directory group '%' not listed in 'Directories'");
            }
        }
    }

    bool IndexThemeReader::readKeyValue(Json& json) 
    {
        if (translations[locale].undefined()) {
            translations[locale] = JsonConst::EmptyObject;
        }

        if (oneOf(key, {"Name", "Comment"})) {
            string translationKey = string("@@") + heading + "::" + key;  
            if (json[key].undefined())  {
                json[key] = translationKey; 
            }
            translations[locale][translationKey] = value;
        }
        else if (key == "Directories") {
            declaredDirectories = splitToSet(value.data(), ',');
        }
        else if (key == "Inherits") {
            json["Inherits"] = JsonConst::EmptyArray;
            for (string inherit : split(value, ',')) {
                json["Inherits"].append(inherit);
            }
        }
        else {
            json[key] = value;
        }
    }

    IconDirectory IndexThemeReader::readDirectoryGroup()
    {
        IconDirectory iconDirectory;
        string sizeType = "Threshold";
        iconDirectory.path = heading;
        int size = -1;
        int threshold = 2;

        while (getNextLine() == KeyValue) {
            if (key == "Size") {
                size = stoi(value);
            }
            else if (key == "Type") {
                if (oneOf(value, {"Fixed", "Scalable", "Threshold"})) {
                    sizeType = value;
                }
                else {
                    throw RuntimeError("Illegal 'Type' value: %s\n", value); 
                }
            }
            else if (key == "Context") {
                iconDirectory.context = value;
            }
            else if (key == "MaxSize") {
                iconDirectory.maxSize = stoi(value);
            }
            else if (key == "MinSize") {
                iconDirectory.minSize = stoi(value);
            }
            else if (key == "Threshold") {
                threshold = stoi(value);
            }
        }

        if (size == -1) {
            throw RuntimeError("No size given");
        }

        if (sizeType == "Fixed") {
            iconDirectory.minSize = iconDirectory.maxSize = size;
        }
        else if (sizeType == "Scalable" && (iconDirectory.minSize == -1 || iconDirectory.maxSize == -1)) {
            throw RuntimeError("With type 'Scalable' both 'MinSize' and 'MaxSize' must be given");
        }
        else if (sizeType == "Threshold") {
            iconDirectory.minSize = size - threshold;
            iconDirectory.maxSize = size + threshold;
        }
        
        return iconDirectory;
    }

    bool IndexThemeReader::oneOf(string str, std::list<std::string> list) {
        for (std::string val : list) if (str == val) return true;
        return false;
    }

}
