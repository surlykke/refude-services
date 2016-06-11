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

#include "refudemainresource.h"
#include "desktop-service/desktopservice.h"
#include "icon-service/iconresourcebuilder.h"
#include "running-applications/runningapplicationsresource.h"
#include "xdg.h"
#include "display/displayresource.h"

int main(int argc, char *argv[])
{
    using namespace org_restfulipc;
    DesktopResources desktopResources;
    try {
        std::string configDir = xdg::config_home() + "/RefudeService";
        system((std::string("mkdir -p ") + configDir).data());

        Service service;
        service.dumpRequests = true;
        service.map("/", std::make_shared<RefudeMainResource>());

        desktopResources.setup(service);

        IconResourceBuilder iconResourceBuilder;
        iconResourceBuilder.buildResources();
        iconResourceBuilder.mapResources(service);

        RunningAppsIcons::ptr runningAppsIcons = std::make_shared<RunningAppsIcons>();
        service.map("/runningappsicons", runningAppsIcons);
        RunningApplicationsResource::ptr runningApplicationsResource = 
            std::make_shared<RunningApplicationsResource>(runningAppsIcons);
        service.map("/runningapplications", runningApplicationsResource, true);

        DisplayResource::ptr displayResource = std::make_shared<DisplayResource>();
        service.map("/display", displayResource);

        std::cout << "Listening on 7938\n";
        service.serve(7938);
        service.wait();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}