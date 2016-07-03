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
        std::vector<std::string> getDataDirs() {
            std::vector<std::string> tmp = split(value("XDG_DATA_DIRS", "/usr/share:/usr/local/share"), ':');
            std::vector<std::string> res;
            for (const std::string& data_dir : tmp) {
                // Some desktop environments seem to add XDG_DATA_HOME to XDG_DATA_DIRS
                // which is not, I think, correct according to the standard
                // see: https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
                if (data_dir != data_home()) {
                    res.push_back(data_dir);
                }
            }

            return res;
        }

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
       
        const std::string& cache_home()
        {
            static std::string res = value("XDG_CACHE_HOME", home() + "/.cache");
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
            static std::vector<std::string> res = getDataDirs();
            return res;
        }

        const std::string& runtime_dir()
        {
            static std::string res = value("XDG_RUNTIME_DIR"); // FIXME: Add fallback
            return res;
        }
    }
} 