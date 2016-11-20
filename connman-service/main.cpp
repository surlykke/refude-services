/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <QDebug>
#include "controller.h"

int main(int argc, char *argv[])
{
    refude::Controller controller(argc, argv);
    return controller.exec();
}
