/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

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
    WebServer::ptr webServer = make_shared<WebServer>("/home/christian/projekter/Aktive/refude/default-applications");

    service.map("/client", webServer, true);
    service.serve(7939);
    service.wait(); 
}
