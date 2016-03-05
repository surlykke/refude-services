#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "service.h"
#include "jsonresource.h"
#include "webserver.h"

int main(int argc, char** argv)
{
    using namespace org_restfulipc;
    Service service;
    WebServer webServer("/home/christian/projekter/Aktive/refude/default-applications");

    service.map("/client", &webServer, true);
    service.serve(7939);
    service.wait(); 
}
