/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "service.h"
#include "json.h"
#include "jsonresourcecollection.h"
#include "notifierresource.h"

namespace refude
{
    class DesktopWatcher;

    class Controller
    {
    public:
        Controller();
        virtual ~Controller();
        void setupAndRun();
        Service service;

    private:
        void update();
        JsonResource::ptr buildAction(Json& application);
        NotifierResource::ptr notifier;
        JsonResourceCollection resources;
        DesktopWatcher* desktopWatcher;
        
        friend class DesktopWatcher; 
    };
}
#endif /* CONTROLLER_H */

