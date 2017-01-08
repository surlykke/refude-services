/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef APPLICATIONCOLLECTOR_H
#define APPLICATIONCOLLECTOR_H
#include <set>
#include "map.h"
#include "json.h"

namespace refude
{
    class ApplicationCollector
    {
    public:
        ApplicationCollector();
        virtual ~ApplicationCollector();
        void collect();
        Map<Json> collectedApplications;
        Map<std::vector<std::string>> defaultApplications;

    private:
        void readDesktopFiles(std::vector<std::string> applicationsDirs);
        void readMimeappsListFile(std::string dir);

    };
}
#endif /* APPLICATIONCOLLECTOR_H */

