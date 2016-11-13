/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the GPL2 file for a copy of the license.
*/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <set>
#include <string>
#include <vector>
#include <refude/map.h>

namespace refude
{
    typedef Map<std::vector<std::string>> AppLists;
    typedef Map<std::set<std::string>> AppSets;
}


#endif /* TYPEDEFS_H */

