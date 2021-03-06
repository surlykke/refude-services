/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef XDG_H
#define XDG_H
#include <vector>
#include <string>

namespace refude
{
    namespace xdg
    {
        const std::string& home();
        const std::string& config_home();
        const std::string& cache_home();
        const std::vector<std::string>& config_dirs();
        const std::string& data_home();
        const std::vector<std::string>& data_dirs();
        const std::string& runtime_dir();
    }
}
#endif /* XDG_H */

