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

#include "xdg.h"
#include "desktopservice.h"

int main(int argc, char *argv[])
{
    using namespace org_restfulipc;
    DesktopResources desktopResources;
    try {
        std::string configDir = xdg::config_home() + "/RefudeService";
        system((std::string("mkdir -p ") + configDir).data());
        
        Service service;
        service.dumpRequests = true;
        desktopResources.setup(service);

        std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.desktop-service";
        service.serve(socketPath.data());
        service.wait();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}