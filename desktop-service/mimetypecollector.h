/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef MIMETYPECOLLECTOR_H
#define MIMETYPECOLLECTOR_H
#include <set>
#include <refude/map.h>
#include <refude/json.h>

namespace refude 
{
    struct MimetypeCollector
    {
        MimetypeCollector();
        virtual ~MimetypeCollector();
        void collect(); 
        void addAssociations(Json& applicationArray);
        void addDefaultApplications(Map<std::vector<std::string>>& defaultApplications);
        
        Json jsonArray;
    };
}

#endif /* MIMETYPECOLLECTOR_H */

