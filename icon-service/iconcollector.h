/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the GPL2 file for a copy of the license.
*/

#ifndef ICONCOLLECTOR_H
#define ICONCOLLECTOR_H
#include <limits.h>
#include <string>

#include <refude/json.h>

#include "types.h"

struct __dirstream;
typedef struct __dirstream DIR;

namespace refude
{
    struct IconCollector
    {
        IconCollector(std::string directoryPath, Json& iconDirectoryJson);
        virtual ~IconCollector();
        void collectInto(IconMap& iconMap);
    
    private: 
        std::string directoryPath;
        Json& iconDirJson;
        char buffer[PATH_MAX];
    };

}
#endif /* ICONCOLLECTOR_H */

