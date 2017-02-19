/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <stdlib.h>
#include <iostream>
#include "service.h"
#include "errorhandling.h"

#include "xdg.h"
#include "controller.h"

int main(int argc, char *argv[])
{
    refude::WmService::run();
}
