#include <iostream>
#include <iostream>
#include <time.h>
#include <unistd.h>

#include "service.h"
#include "genericresource.h"

using namespace org_restfulipc;
using namespace std;

int main(int argc, char *argv[])
{
    char json[256];
    GenericResource resource("{}");

    Service service("org.restfulipc.examples.Dynamic");
    service.resourceMap.map("/res", &resource);

    for (int i = 0; i < 10; i++) {
        sprintf(json,
                "{\n"
                "    time: %d\n"
                "}\n",
                time(NULL));
        cout << i << "\n";
        resource.update(json);
        sleep(1);
    }
    cout << "exiting\n";
}
