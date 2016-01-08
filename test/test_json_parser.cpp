#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "jsonwriter.h"
#include "json.h"
#include "jsonreader.h"

using namespace org_restfulipc;

int main()
{
    int dataFd = open("./data.json", O_RDONLY | O_CLOEXEC); 
    char buf[2048];
    int bufSize = 0;
    int bytesRead = 0;
    while ((bytesRead = read(dataFd, buf + bufSize, 2048 - bufSize)) > 0) 
        bufSize += bytesRead;
    buf[bufSize++] = '\0';
    std::cout << "Parsing:\n" << buf << "\n";
    Json json = JsonReader(buf).read();
    std::cout << JsonWriter(&json).buffer.data << "\n";
}
