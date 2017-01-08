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
#include "notifierresource.h"
#include "jsonresourcecollection.h"
#include "map.h"

#include "runningappsicons.h"

namespace refude 
{
    class Controller
    {
    public:
        Controller();
        ~Controller();
        void run();

    private:
        void update();
        Map<Json> collectWindows();
        Json buildDisplay();
        Json collectActions(Map<Json>& windows);
        Service dispatcher;
        NotifierResource::ptr notifier;
        JsonResourceCollection jsonResources;
        RunningAppsIcons::ptr iconsResource;
    };
}
#endif // CONTROLLER_H
