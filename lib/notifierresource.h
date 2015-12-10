#ifndef NOTIFIERRESOURCE_H
#define NOTIFIERRESOURCE_H

#include <mutex>

#include "abstractresource.h"

namespace org_restfulipc {

    class NotifierResource : public AbstractResource
    {
    public:

        enum class Event : int
        {
            Updated,
            Removed
        };

        NotifierResource();

        virtual void handleRequest(int &socket, const HttpMessage& request);
        void notifyClients(Event event, const char* pathOfResource);

    private:
        void addClient(int socket);

        int* mClientSockets;
        int  mNumberOfClientSockets;
        int  mClientSocketsCapacity;
        std::mutex mMutex;
    };

}
#endif // NOTIFIERRESOURCE_H
