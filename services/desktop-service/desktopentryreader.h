/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef DESKTOPENTRYREADER_H
#define DESKTOPENTRYREADER_H
#include <fstream>
#include <string>
#include <list>

#include <ripc/json.h>
#include "inireader.h"

namespace org_restfulipc 
{
    class DesktopEntryReader : private IniReader
    {
    public:
        DesktopEntryReader(std::string desktopFilePath);
        virtual ~DesktopEntryReader();
        Json json;
    
    private:
        void read();
        void readMainGroup();
        void readGroup();

        void readKeyValues(Json& json);
        bool readKeyValue(Json& json);
        void handleBool(Json&, std::string value);
        std::list<std::string> toList(std::string value);

        bool keyOneOf(std::list<std::string> list);
    };
}

#endif /* DESKTOPENTRYREADER_H */

