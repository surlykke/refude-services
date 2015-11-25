#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jsonwriter.h"
#include "json.h"

using namespace org_restfulipc;

int main()
{

    uint32_t bufsize = 100000000;
    char* buf = new char[bufsize];
    int bufend = 0;

    int bytesRead;
    while ((bytesRead = read(0, buf + bufend, bufsize - bufend)) > 0) {
        bufend += bytesRead;
    }
    if (bytesRead < 0) throw errno;
    buf[bufend] = 0;
    Json json;
    json << buf;
    JsonWriter(&json).write();
}
