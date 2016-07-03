/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <stdlib.h>
#include <iostream>
#include <ripc/service.h>
#include <ripc/errorhandling.h>

#include "runningapplicationsresource.h"
#include "displayresource.h"
#include "xdg.h"

int main(int argc, char *argv[])
{
    using namespace org_restfulipc;
    try {
        std::string configDir = xdg::config_home() + "/RefudeService";
        system((std::string("mkdir -p ") + configDir).data());

        Service service;
        service.dumpRequests = true;
        RunningAppsIcons::ptr runningAppsIcons = std::make_shared<RunningAppsIcons>();
        service.map("/runningappsicons", runningAppsIcons);
        RunningApplicationsResource::ptr runningApplicationsResource = 
            std::make_shared<RunningApplicationsResource>(runningAppsIcons);
        service.map("/runningapplications", runningApplicationsResource, true);

        DisplayResource::ptr displayResource = std::make_shared<DisplayResource>();
        service.map("/display", displayResource);

        std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.windowmanager-service";
        service.serve(socketPath.data());
        service.wait();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}