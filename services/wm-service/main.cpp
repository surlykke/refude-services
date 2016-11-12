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

#include "xdg.h"
#include "controller.h"

int main(int argc, char *argv[])
{
    using namespace org_restfulipc;
    try {
        /*std::string configDir = xdg::config_home() + "/RefudeService";
        system((std::string("mkdir -p ") + configDir).data());*/

        std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.wm-service";
        Controller controller;
        controller.dispatcher.serve(socketPath.data());
        controller.run();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}