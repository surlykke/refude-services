/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <set>
#include <string>

namespace org_restfulipc 
{
    using namespace std;

    vector<string> split(string str, char c);
    set<string> splitToSet(string str, char c);
    string value(const string& envVarName, const string& fallback = "");
}
#endif /* UTILS_H */

