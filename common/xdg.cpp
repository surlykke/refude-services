/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <ripc/errorhandling.h>
#include <ripc/utils.h>

#include "xdg.h"

namespace org_restfulipc
{

    namespace xdg
    {

        const string& home() 
        {
            static string res = value("HOME");
            return res; 
        }

        const string& config_home()
        {
            static string res = value("XDG_CONFIG_HOME", home() + "/.config");
            return res; 
        }
        
        const vector<string>& config_dirs() 
        {
            static vector<string> res = split(value("XDG_CONFIG_DIRS", "/etc/xdg"), ':');
            return res;
        }

        const string& data_home()
        {
            static string res = value("XDG_DATA_HOME", home() + "/.local/share");
            return res;
        }

        const vector<string>& data_dirs() 
        {
            static vector<string> res = split(value("XDG_DATA_DIRS", "/usr/share:/usr/local/share"), ':');
            return res;
        }
    }
} 