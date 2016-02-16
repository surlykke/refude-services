#include <iostream>
#include "desktopservice.h"

using namespace org_restfulipc;
int main(int argc, char *argv[])
{
    DesktopService desktopService;
    desktopService.serve(7938);
    desktopService.wait();
}
