/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the GPL2 file for a copy of the license.
*/

#ifndef THEMEREADER_H
#define THEMEREADER_H
#include <string>
#include "json.h"
#include "inireader.h"

namespace refude
{
    class ThemeReader : private IniReader
    {
    public:
        ThemeReader(Json& theme, const std::string& dirPath);
        virtual ~ThemeReader();
        Json& themeJson;

    private:
        void read();
        bool oneOf(std::string str, std::list<std::string> list, bool caseSensitive = false);
        const std::string& dirPath; 
    };
    
}
#endif /* THEMEREADER_H */

