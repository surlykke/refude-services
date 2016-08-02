/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <ripc/service.h>
#include <ripc/notifierresource.h>
#include <ripc/collectionresource.h>
#include "xdg.h"
#include "powerapplication.h"

int main(int argc, char *argv[])
{
    using namespace org_restfulipc;
    
    Service service;
   
    CollectionResource::ptr devicesResource = std::make_shared<CollectionResource>("deviceId");
    service.map(devicesResource, true, "devices");
   
    NotifierResource::ptr notifierResource = std::make_shared<NotifierResource>();
    service.map(notifierResource, "notify");

    std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.power-service";
    service.serve(socketPath.data());
     
    PowerApplication powerApplication(devicesResource, notifierResource, argc, argv);
    powerApplication.collectJsons();
    powerApplication.exec();
}

