/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef ICONCOLLECTOR_H
#define ICONCOLLECTOR_H
#include <limits.h>
#include <string>

#include "icontheme.h"
struct __dirstream;
typedef struct __dirstream DIR;
namespace org_restfulipc
{
    using namespace std;
    
    struct IconCollector
    {
        IconCollector(string directoryPath);
        virtual ~IconCollector();
        map<string, IconInstance> collectedIcons;
        DIR* dir;
        char buffer[PATH_MAX];
    };

}
#endif /* ICONCOLLECTOR_H */
