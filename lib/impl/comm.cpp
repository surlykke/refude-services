#include <sys/socket.h>

#include "errorhandling.h"

#include "comm.h"

namespace refude
{
    void sendFully(int socket, const char* data, int nbytes)
    {
        for (int bytesWritten = 0; bytesWritten < nbytes;) {
            int n = send(socket, data + bytesWritten, nbytes - bytesWritten, MSG_NOSIGNAL);
            if (n < 0) throw C_Error();
            bytesWritten += n;
        }
    }
}
