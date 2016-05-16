/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jsonwriter.h"
#include "json.h"

int main()
{

    Json json;
    json = JsonConst::EmptyObject;
    json["foo"] = 7;
    json["baa"] = JsonConst::FALSE;
    json.take("foo");
    std::cout << json.size() << "\n";
    std::cout << JsonWriter(json).buffer.data << "\n"; 
    std::cout << "\n";

    json = JsonConst::EmptyArray;
    json.append( 1);
    json.append( 2);
    json.append( 3);
    json.take(2);
    Json taken = json.take(0);
    std::cout << "At 0: " << (double)taken << "\n";
    std::cout << JsonWriter(json).buffer.data << "\n";
}
