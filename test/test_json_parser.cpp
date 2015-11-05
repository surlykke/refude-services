#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "jsondoc.h"

using namespace org_restfulipc;

int main()
{
    try {
        char buf[8192];
        char buf2[8192];

        int bufsize = 0;
        int bytesRead;
        while ((bytesRead = read(0, buf + bufsize, 8191 - bufsize)) > 0) {
            bufsize += bytesRead;
        }
        if (bytesRead < 0) throw errno;
        printf("Read %d bytes\n", bufsize);
        printf("buf: %s\n", buf);
        buf[bufsize] = 0;
        JsonDoc doc(buf);
        doc.serialize(buf2);
        std::cout << "buf2:\n" << buf2 << "\n";
    }
    catch (int errorNumber) {
        printf("Caught errorNumber: %s\n", strerror(errorNumber));
    }
    catch (RuntimeError ex) {
        printf("Caught UnexpectedCharacter: %s\n", ex.what());
    }
}
