/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <iostream>
#include "desktopservice.h"
#include <string>
using namespace org_restfulipc;
int main(int argc, char *argv[])
{
    DesktopService service;
    std::cout << "Serving 7938\n";
    service.serve(7938);
    service.wait();
}
