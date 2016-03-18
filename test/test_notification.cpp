#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "service.h"
#include "genericresource.h"
#include "notifierresource.h"

int main(int argc, char** argv)
{

    using namespace org_restfulipc;
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
