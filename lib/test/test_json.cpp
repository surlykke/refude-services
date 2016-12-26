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

#include "jsonwriter.h"
#include "json.h"

using namespace refude;
int main()
{

   std::cout << "Start building\n";
    Json json = JsonConst::EmptyObject;
    std::vector<Pair<Json>> v;
    for (int i = 0; i < 5000000; i++) {
        v.push_back(Pair<Json>("foo", 1));
        v.push_back(Pair<Json>("baa", 2));
        v.push_back(Pair<Json>("yxy", 3));
    }
    json.append(std::move(v));
    std::cout << "Start sorting\n";
    std::cout << JsonWriter(json).buffer.data() << "\n";
}
