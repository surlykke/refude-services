/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <ripc/notifierresource.h>
#include <ripc/jsonresource.h>
#include <ripc/collectionresource.h>
#include <ripc/service.h>

#include "runningappsicons.h"

namespace org_restfulipc 
{
    class Controller
    {
    public:
        Controller();
        ~Controller();
        void run();
        Service dispatcher;
    
    private:
        void updateResources();
        NotifierResource::ptr notifier;
        CollectionResource::ptr windowsResource;
        JsonResource::ptr displayResource;
        RunningAppsIcons::ptr iconsResource;
    };
}
#endif // CONTROLLER_H
