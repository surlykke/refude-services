#include <sys/uio.h>
#include <unistd.h>

#include "notifierresource.h"
#include "errorhandling.h"

namespace org_restfulipc {

    NotifierResource::NotifierResource() :
        AbstractResource(),
        mClientSockets(0),
        mNumberOfClientSockets(0),
        mClientSocketsCapacity(0),
        mMutex()
    {
        if (!(mClientSockets = (int*) calloc(128, sizeof(int)))) {
            throw C_Error();
        }
        mNumberOfClientSockets = 0;
        mClientSocketsCapacity = 128;
    }

    void NotifierResource::handleRequest(int &socket, const HttpMessage &request)
    {
        static const char* response =
                "HTTP/1.1 200 OK\r\n"
                "Connection: keep-alive\r\n"
                "Content-Type: text/event-stream\r\n"
                "Transfer-Encoding: chunked\r\n"
                "\r\n";

        static int responseLength = strlen(response);

        std::cout << "Into NotifierResource::handleRequest\n";

        if (request.method != Method::GET) {
            throw Status::Http405;
        }

        std::cout << "Writing: " << response << "\n";
        int bytesWritten = 0;
        do {
            int bytes;
            if ((bytes = write(socket, response + bytesWritten, responseLength - bytesWritten)) < 0) {
                throw C_Error();
            }
            bytesWritten += bytes;
        }
        while (bytesWritten < responseLength);

        addClient(socket);
    }

    void NotifierResource::addClient(int socket)
    {
        std::cout << "Adding client: " << socket << "\n";
        std::unique_lock<std::mutex> lock(mMutex);

        if (mNumberOfClientSockets == mClientSocketsCapacity) {
            if (!(mClientSockets = (int*) realloc(mClientSockets, 2*mClientSocketsCapacity))) {
                throw C_Error();
            }
            mClientSocketsCapacity = 2*mClientSocketsCapacity;
        }

        mClientSockets[mNumberOfClientSockets++] = socket;
    }


    void NotifierResource::notifyClients(Event event, const char* pathOfResource)
    {
        char data[300];
        const char* eventLine = event == Event::Updated ? "event:updated\n" : "event:removed\n";
        const char* dataPrefix = "data:";

        int pathLength = strlen(pathOfResource);
        if ( pathLength > 256) {
            throw RuntimeError("Path too long");
        }
        int chunkLength = strlen(eventLine) + strlen(dataPrefix) + pathLength + 2;
        int dataLength = sprintf(data, "%x\r\n%s%s%s\n\n\r\n", chunkLength, eventLine, dataPrefix, pathOfResource);
        {
            std::unique_lock<std::mutex> lock(mMutex);

            for (int i = 0; i < mNumberOfClientSockets; i++) {
                std::cout << "Writing \n" << data << "-----\n";
                if (write(mClientSockets[i], data, dataLength) < dataLength) {
                    // We close, both if error and if sendbuffer full, which we take to
                    // indicate the client is not consuming... The client will then have to reconnnect
                    while (close(mClientSockets[i]) < 0 && errno == EINTR);
                    mClientSockets[i] = -1;
                }
            }


            // Reap the closed and compactify
            int closed = 0;
            for (int i = 0; i < mNumberOfClientSockets; i++) {
                if (mClientSockets[i] == -1) {
                    closed++;
                }
                else {
                    mClientSockets[i - closed] = mClientSockets[i];
                }
            }
            mNumberOfClientSockets = mNumberOfClientSockets - closed;
        }

    }


}
