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
    try {
        char json[256];
        GenericResource resource("{}");
    
        Service service("org.restfulipc.examples.Dynamic");
        service.resourceMap.map("/res", &resource);
        cout << "Starting\n";
        service.start();
        for (int i = 0; i < 5; i++) {
            sprintf(json,
                    "{\n"
                    "    time: %d\n"
                    "}\n",
                    time(NULL));
            cout << "Updating\n";
            resource.update(json);
            sleep(3);
        }
        service.stop();
        
   }
    catch (int errorNumber) {
        if (errorNumber) {
            cout << strerror(errorNumber) << "\n";
        }
    }
}
