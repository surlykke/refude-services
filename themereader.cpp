/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <dirent.h>
#include "indexthemereader.h"

#include "themereader.h"
#include "iconcollector.h"
namespace org_restfulipc
{
    ThemeReader::ThemeReader(Json& theme, const string& dirPath) :
        theme(theme),
        dirPath(dirPath)
    {
        read();
    }

    ThemeReader::~ThemeReader()
    {
    }
       
    void ThemeReader::read()
    {
        IndexThemeReader indexThemeReader(dirPath + "/index.theme");
        if (indexThemeReader.lineType == IniReader::EndOfFile) {
            return;
        }
        
        indexThemeReader.read();
        for (const IconDirectory& p : indexThemeReader.iconDirectories) {
            string path = dirPath + '/' + p.path; 
            IconCollector iconCollector(path);
            iconCollector.collectedIcons.each([this, &p](const char* iconName, Json& icon) {
                icon["minSize"] = p.minSize;
                icon["maxSize"] = p.maxSize;
               
                if (! theme["Icons"].contains(iconName)) {
                    theme["Icons"][iconName] = JsonConst::EmptyArray;
                }
                theme["Icons"][iconName].append(move(icon));
            });
        }
    }
}
