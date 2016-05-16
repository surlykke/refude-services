/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "service.h"
#include "genericresource.h"
#include "notifierresource.h"

int main(int argc, char** argv)
{
    
    Service service;
    NotifierResource::ptr notifier = make_shared<NotifierResource>();
    GenericResource::ptr resource = make_shared<GenericResource>("", notifier);
    service.map("/res", resource, true);
    service.map("/notify", notifier);
    service.serve(7938);

    char json[512];
    for(;;) {
        sprintf(json, "{\"time\" : %Ld}\n", time(NULL)) ;
        std::cout << json;
        resource->update(json);
        sleep(3);
    }
}
