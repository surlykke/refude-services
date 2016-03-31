/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
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