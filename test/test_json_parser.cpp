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
#include <fcntl.h>
#include <iostream>
#include "jsonwriter.h"
#include "json.h"
#include "jsonreader.h"

using namespace refude;
int main()
{
    try {
        int dataFd = open("./data.json", O_RDONLY | O_CLOEXEC); 
        char buf[2048];
        int bufSize = 0;
        int bytesRead = 0;
        while ((bytesRead = read(dataFd, buf + bufSize, 2048 - bufSize)) > 0) 
            bufSize += bytesRead;
        buf[bufSize++] = '\0';
        std::cout << "Parsing " << buf << "\n";
        Json json = JsonReader(buf).read();
        std::cout << JsonWriter(json).buffer.data() << "\n";
    }
    catch (RuntimeError re) {
        std::cout << "Caught error: " << re.errorMsg << "\n";
    }
}
