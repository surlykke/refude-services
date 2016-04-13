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
    ThemeReader::ThemeReader(IconTheme& iconTheme, const string& dirPath) :
        iconTheme(iconTheme),
        dirPath(dirPath)
    {
        read();
        std::cout << "Collected " << iconTheme.size() << " icons from " << dirPath << "\n";
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
            for (auto& pair : iconCollector.collectedIcons) {
                const string& iconName = pair.first;
                IconInstance& iconInstance = pair.second;
               
                iconInstance.minSize = p.minSize;
                iconInstance.maxSize = p.maxSize;
                
                iconTheme[iconName].push_back(move(iconInstance));
            }
        }
    }
}
