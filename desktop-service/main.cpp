/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */
#include <stdlib.h>
#include <iostream>
#include <refude/service.h>
#include <refude/errorhandling.h>

#include <refude/xdg.h>
#include "controller.h"

int main(int argc, char *argv[])
{
    using namespace refude;
    Controller controller;
    try {
        controller.setupAndRun();
        std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.desktop-service";
        controller.service.dumpRequests = true;
        controller.service.serve(socketPath.data());
        controller.service.wait();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}