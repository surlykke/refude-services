/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <iostream>
#include <ripc/service.h>
#include <ripc/errorhandling.h>

#include "refudemainresource.h"
#include "desktop-service/desktopservice.h"
#include "icon-service/iconresourcebuilder.h"

using namespace org_restfulipc;
using namespace std;
int main(int argc, char *argv[])
{
    try {
        Service service;
        service.dumpRequests = true;
        service.map("/", make_shared<RefudeMainResource>());

        DesktopResources desktopResources;
        desktopResources.setup(service);

        IconResourceBuilder iconResourceBuilder;
        iconResourceBuilder.buildResources();
        iconResourceBuilder.mapResources(service);


        cout << "Listening on 7938\n";
        service.serve(7938);
        service.wait();
    }
    catch (RuntimeError re) {
        cerr << re.what() << "\n";
        re.printStackTrace();
        cerr << "\n";
    }
}
