#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "service.h"
#include "genericresource.h"
#include "notifierresource.h"

int main(int argc, char** argv)
{

    using namespace org_restfulipc;
    Service service(7938);
    NotifierResource notifier;
    GenericResource resource("", &notifier);
    service.resourceMap.map("/res", &resource);
    service.resourceMap.map("/res/notify", &notifier);

    char json[512];
    for(;;) {
        sprintf(json, "{\"time\" : %Ld}\n", time(NULL)) ;
        std::cout << json;
        resource.update(json);
        sleep(3);
    }
}
