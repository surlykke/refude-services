#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jsonparser.h"

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
        Heap heap;
        JsonParser parser(buf, &heap);
        AbstractJson* json = parser.read();
        json->serialize();
        char *dest = buf2;
        json->serialize(dest);
        printf("buf2: %s\n", buf2);
    }
    catch (int errorNumber) {
        printf("Caught errorNumber: %s\n", strerror(errorNumber));
    }
    catch (RuntimeError ex) {
        printf("Caught UnexpectedCharacter: %s\n", ex.what());
    }
}
