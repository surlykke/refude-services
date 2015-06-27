#include <iostream>
#include <iostream>
#include <time.h>
#include <unistd.h>

#include "service.h"
#include "genericresource.h"

int main(int argc, char *argv[])
{
	char json[256];
	GenericResource resource("{}");
	
	ServiceListener serviceListener;
	serviceListener.setup("org.restfulipc.examples.Dynamic");
	serviceListener.map(&resource, "/res");
	
	while (true) {
		sprintf(json, "{time: %d}", time(NULL));
		std::cout << "Updating to " << json << "\n";
		resource.update(json);	
		sleep(100);	
	}
}
