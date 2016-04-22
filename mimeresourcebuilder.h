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
    using namespace std;
    class MimeResourceBuilder
    {
    public:
        MimeResourceBuilder();
        virtual ~MimeResourceBuilder();
        void build();
        void addAssociationsAndDefaults(const AppSets& associations, const AppLists& defaults);
        void mapResources(Service& service, NotifierResource::ptr notifier);
    
    private:
        Json root;
        Map<Json> jsons;
    };
}
#endif /* MIMERESOURCEBUILDER_H */

