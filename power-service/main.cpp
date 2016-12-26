/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include "server.h"
#include "notifierresource.h"
#include "collectionresource.h"
#include "xdg.h"
#include "powerapplication.h"

int main(int argc, char *argv[])
{
    using namespace refude;
    
    PowerApplication powerApplication(argc, argv);
    powerApplication.collectActionJsons();
    powerApplication.collectDeviceJsons();

    Server service;
    service.mapPrefix(powerApplication.actionsResource, "/actions");
    service.map(powerApplication.devicesResource, "/devices");
    service.map(powerApplication.notifierResource, "/notify");

    std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.power-service";
    service.serve(socketPath.data());

    powerApplication.exec();
}

