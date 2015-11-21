#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "jsondoc.h"
#include <vector>
#include <map>

using namespace org_restfulipc;

int main()
{
    uint32_t bufsize = 100000000;
    char* buf = (char*) calloc(bufsize, 1);
    int bufend = 0;

    int bytesRead;
    while ((bytesRead = read(0, buf + bufend, bufsize - bufend)) > 0) {
        bufend += bytesRead;
    }
    if (bytesRead < 0) throw errno;
    buf[bufend] = 0;
    JsonDoc doc(buf);
    std::cout << "Done reading, writing...\n";
    doc.write();
    std::cout << "Done writing\n";
}
