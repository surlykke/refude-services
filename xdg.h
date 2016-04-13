/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#ifndef XDG_H
#define XDG_H
#include <vector>
#include <string>

namespace org_restfulipc
{
    using namespace std;

    namespace xdg
    {
        const string& home();
        const string& config_home();
        const vector<string>& config_dirs();
        const string& data_home();
        const vector<string>& data_dirs();
    }
}
#endif /* XDG_H */

