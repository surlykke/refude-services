/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
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
    for (int i = 0; i < 5000000; i++) {
        json.append("foo", 1);
        json.append("baa", 2);
        json.append("yxy", 3);
    }
    std::cout << "Start sorting\n";
    std::cout << JsonWriter(json).buffer.data() << "\n";
}
