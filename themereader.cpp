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
namespace org_restfulipc
{
    ThemeReader::ThemeReader(IconTheme& iconTheme, const string& dirPath) :
        iconTheme(iconTheme),
        dirPath(dirPath)
    {
        read();
        std::cout << "Collected " << iconTheme.size() << " icons\n";
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
        for (const auto& p : indexThemeReader.iconDirectories) {
            collectIcons(dirPath, p.second);
        }
    }

    void ThemeReader::collectIcons(string iconThemeDirectoryPath, const IconDirectory& iconDirectory)
    {
        string absPath = iconThemeDirectoryPath + "/" + iconDirectory.path;

        vector<string> files;
        DIR* dir = opendir(absPath.data());
        if (dir == NULL) {
            std::cerr << "Warning: " << absPath.data() << " does not exist\n";
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
                string iconName = fileName.substr(0, fileName.size() - 4); // All endings '.png', '.xpm', and '.svg' 
                                                                           // have same length.
                instance.path = absPath + "/" + dirent->d_name;
                instance.maxSize = iconDirectory.maxSize;
                instance.minSize = iconDirectory.minSize;
                iconTheme[iconName].push_back(instance);
            }
        }
       
        closedir(dir);
    }

}
