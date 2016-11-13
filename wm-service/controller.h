/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <refude/notifierresource.h>
#include <refude/jsonresource.h>
#include <refude/collectionresource.h>
#include <refude/service.h>

#include "runningappsicons.h"

namespace refude 
{
    class Controller
    {
    public:
        Controller();
        ~Controller();
        void run();
        Service dispatcher;
        void buildDisplayResource();
        void updateWindowsResource();
     
    private:
        NotifierResource::ptr notifier;
        CollectionResource::ptr windowsResource;
        JsonResource::ptr displayResource;
        RunningAppsIcons::ptr iconsResource;
        bool windowsResourceStale;
    };
}
#endif // CONTROLLER_H
