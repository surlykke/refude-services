/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "server.h"
#include "json.h"
#include "collectionresource.h"
#include "notifierresource.h"

namespace refude
{
    class DesktopWatcher;

    class Controller
    {
    public:
        Controller();
        virtual ~Controller();
        void update();

        Server service;
    private:
        
        CollectionResource::ptr applicationsResource;
        CollectionResource::ptr mimetypesResource;
        NotifierResource::ptr notifier; 
    };
}
#endif /* CONTROLLER_H */

