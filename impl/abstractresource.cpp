/* 
 * File:   AbstractResource.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. marts 2015, 09:42
 */
#include <string.h>
#include <unistd.h>
#include <iterator>
#include <sys/socket.h>
#include "errorhandling.h"
#include "abstractresource.h"

namespace org_restfulipc
{

    void AbstractResource::sendFully(int socket, const char* data, int nbytes) {
        for (int bytesWritten = 0; bytesWritten < nbytes; ) {
            int n = send(socket, data + bytesWritten, nbytes - bytesWritten, MSG_NOSIGNAL);
            if (n < 0) throw C_Error();
            bytesWritten += n;
        }
    }

}