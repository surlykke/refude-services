#ifndef COMM_H
#define COMM_H

#include "httpprotocol.h"
#include "buffer.h"

namespace refude
{
    void sendBuffer(int socket, const Buffer& buf);
    void sendStatus(int socket, HttpCode status);
    void sendFile(int socket, const char* path, const char* mimetype = 0);
    void sendString(int socket, const char* str, int size);
}

#endif // COMM_H
