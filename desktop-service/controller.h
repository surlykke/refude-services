/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <refude/service.h>
#include <refude/json.h>
#include <refude/collectionresource.h>
#include <refude/notifierresource.h>

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
        
        CollectionResource::ptr applicationsResource;
        CollectionResource::ptr mimetypesResource;
        NotifierResource::ptr notifier; 

        DesktopWatcher* desktopWatcher;
        
        friend class DesktopWatcher; 
    };
}
#endif /* CONTROLLER_H */

