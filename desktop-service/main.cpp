/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <stdlib.h>
#include <iostream>
#include "server.h"
#include "errorhandling.h"

#include "xdg.h"
#include "controller.h"
#include "desktopwatcher.h"
#include "app.h"

int main(int argc, char *argv[])
{
    using namespace refude;
    try {
        std::cout << "Controller()\n";
        Controller controller;
        /*DesktopWatcher watcher(controller, true);
        std::thread t(&DesktopWatcher::start, &watcher);*/
        std::string socketPath = xdg::runtime_dir() + "/org.restfulipc.refude.desktop-service";
        controller.service.serve(socketPath.data());
        App::run();
    }
    catch (RuntimeError re) {
        std::cerr << re.what() << "\n";
        re.printStackTrace();
        std::cerr << "\n";
    }
}
