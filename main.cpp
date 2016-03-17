#include <iostream>
#include "desktopresourcebuilder.h"
#include "mimeresourcebuilder.h"

using namespace org_restfulipc;
int main(int argc, char *argv[])
{
    Service service;
    MimeappsListCollector mimeappsListCollector;
    DesktopResourceBuilder desktopResourceBuilder(&service, mimeappsListCollector);
    desktopResourceBuilder.build();

    MimeResourceBuilder mimeResourceBuilder(&service);
    mimeResourceBuilder.build("/usr/share/mime/packages/freedesktop.org.xml",
                              mimeappsListCollector.associations,
                              mimeappsListCollector.defaults);

    service.serve(7938);
    service.wait();
}
