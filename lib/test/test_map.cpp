/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include <refude/map.h>
#include <refude/json.h>
#include <refude/jsonwriter.h>

using namespace refude;
int main()
{
    Map<Map<Json>> maps;

    Map<Json>& hey = maps["hey"];

    std::cout << JsonWriter(hey["there"]).buffer.data() << std::endl;
}
