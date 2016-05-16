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

        const std::string& home() 
        {
            static std::string res = value("HOME");
            return res; 
        }

        const std::string& config_home()
        {
            static std::string res = value("XDG_CONFIG_HOME", home() + "/.config");
            return res; 
        }
        
        const std::vector<std::string>& config_dirs() 
        {
            static std::vector<std::string> res = split(value("XDG_CONFIG_DIRS", "/etc/xdg"), ':');
            return res;
        }

        const std::string& data_home()
        {
            static std::string res = value("XDG_DATA_HOME", home() + "/.local/share");
            return res;
        }

        const std::vector<std::string>& data_dirs() 
        {
            static std::vector<std::string> res = split(value("XDG_DATA_DIRS", "/usr/share:/usr/local/share"), ':');
            return res;
        }
    }
} 