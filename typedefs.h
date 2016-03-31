/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <set>
#include <list>
#include <string>

namespace org_restfulipc
{
    using namespace std;

    typedef map<string, vector<string> > AppLists;
    typedef map<string, set<string> > AppSets;

}


#endif /* TYPEDEFS_H */

