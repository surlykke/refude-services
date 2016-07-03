/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <ripc/service.h>
#include <ripc/notifierresource.h>
#include "xdg.h"
#include "powerapplication.h"

int main(int argc, char *argv[])
{
    using namespace org_restfulipc;
    
    Service service;

    NotifierResource::ptr notifierResource = std::make_shared<org_restfulipc::NotifierResource>();
    service.map("/notify", notifierResource);
    
    PowerApplication powerApplication(service, notifierResource, argc, argv);

    std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.power-service";
    service.serve(socketPath.data());
    powerApplication.exec();
}

