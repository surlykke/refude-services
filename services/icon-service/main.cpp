/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <stdlib.h>
#include <iostream>
#include <refude/service.h>
#include <refude/errorhandling.h>

#include "iconresourcebuilder.h"
#include <refude/xdg.h>

int main(int argc, char *argv[])
{
    using namespace refude;
    try {
        Service service;
        IconResourceBuilder iconResourceBuilder;
        iconResourceBuilder.buildResources();
        iconResourceBuilder.mapResources(service);
        std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.icon-service";
        service.serve(socketPath.data());
        service.wait();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}