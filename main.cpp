#include <iostream>
#include "desktopservice.h"

using namespace org_restfulipc;
int main(int argc, char *argv[])
{
    std::cout << "create service..\n";
    DesktopService service;
    std::cout << "Serving 7938\n";
    service.serve(7938);
    service.wait();
}
