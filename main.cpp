#include <iostream>
#include "desktopresourcebuilder.h"
#include "mimeresourcebuilder.h"

using namespace org_restfulipc;
int main(int argc, char *argv[])
{
    Service service;
    DesktopResourceBuilder desktopResourceBuilder(&service);
    desktopResourceBuilder.build();

    MimeResourceBuilder mimeResourceBuilder(&service);
    mimeResourceBuilder.build("/usr/share/mime/packages/freedesktop.org.xml");

    service.serve(7938);
    service.wait();
}
