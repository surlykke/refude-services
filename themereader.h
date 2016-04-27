/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef THEMEREADER_H
#define THEMEREADER_H
#include <magic.h>
#include <string>
#include <ripc/json.h>
#include "icontheme.h"
#include "indexthemereader.h"

namespace org_restfulipc
{
    using namespace std;

    class ThemeReader
    {
    public:
        ThemeReader(Json& theme, const string& dirPath);
        virtual ~ThemeReader();
        Json& theme;
    private:
        void read();
        const string& dirPath; 
    };
    
}
#endif /* THEMEREADER_H */

