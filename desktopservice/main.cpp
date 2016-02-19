#include <iostream>
#include "desktopresourcebuilder.h"
#include "mimeresourcebuilder.h"

using namespace org_restfulipc;
int main(int argc, char *argv[])
{
    Service service;
    MimeappsListReader mimeappsListReader;
    DesktopResourceBuilder desktopResourceBuilder(&service, mimeappsListReader);
    desktopResourceBuilder.build();

    MimeResourceBuilder mimeResourceBuilder(&service);
    mimeResourceBuilder.build("/usr/share/mime/packages/freedesktop.org.xml",
                              mimeappsListReader.associations,
                              mimeappsListReader.defaults);

    service.serve(7938);
    service.wait();
}
