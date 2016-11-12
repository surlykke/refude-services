/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <refude/service.h>
#include <refude/notifierresource.h>
#include <refude/collectionresource.h>
#include <refude/xdg.h>
#include "powerapplication.h"

int main(int argc, char *argv[])
{
    using namespace refude;
    
    PowerApplication powerApplication(argc, argv);
    powerApplication.collectActionJsons();
    powerApplication.collectDeviceJsons();

    Service service;
	service.dumpRequests = true;   
    service.map(powerApplication.actionsResource, true, "actions");
    service.map(powerApplication.devicesResource, true, "devices");
    service.map(powerApplication.notifierResource, "notify");

    std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.power-service";
    service.serve(socketPath.data());

    powerApplication.exec();
}

