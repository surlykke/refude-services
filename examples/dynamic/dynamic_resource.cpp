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
        service.start();
        service.resourceMap.map("/res", &resource);

		while (true) {
			sprintf(json, 
				    "{\n"
					"    time: %d\n"
			        "}\n", 
				    time(NULL));
			std::cout << "Updating to " << json << "\n";
			resource.update(json);	
			sleep(3);	
		}
	}
	catch (int errorNumber) {
        cout << strerror(errorNumber) << "\n";
	}
}
