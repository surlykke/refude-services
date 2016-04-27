/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef INDEXTHEMEREADER_H
#define INDEXTHEMEREADER_H

#include <string>
#include <set>
#include <map>
#include <ripc/json.h>
#include "inireader.h"

namespace org_restfulipc 
{
    using namespace std;

    struct IconDirectory
    {
        string path;
        string context; 
        int minSize;
        int maxSize;
    };

    class IndexThemeReader : public IniReader
    {
    public:
        IndexThemeReader(string indexThemeFilePath);
        virtual ~IndexThemeReader();
        void read();
        Json json;
        Json jsonFull; 
        vector<IconDirectory> iconDirectories;
    private:
        bool readKeyValue();
        IconDirectory readDirectoryGroup();
        bool oneOf(string str, std::list<std::string> list);
        
        set<string> declaredDirectories;
    };
}
#endif /* INDEXTHEMEREADER_H */

