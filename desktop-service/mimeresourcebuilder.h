/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef MIMERESOURCEBUILDER_H
#define MIMERESOURCEBUILDER_H
#include <ripc/json.h>
#include <ripc/service.h>
#include <ripc/notifierresource.h>
#include <ripc/json.h>

#include "typedefs.h"
namespace org_restfulipc
{
    class MimeResourceBuilder
    {
    public:
        MimeResourceBuilder();
        virtual ~MimeResourceBuilder();
        void build();
        void addAssociationsAndDefaults(Map<Json>& desktopJsons,  
                                        Map<std::vector<std::string>>& defaultApplications);
        Map<Json> mimetypeJsons;
    };
}
#endif /* MIMERESOURCEBUILDER_H */
