#ifndef NOTIFIERRESOURCE_H
#define NOTIFIERRESOURCE_H

#include <mutex>
#include <vector>

#include "abstractresource.h"
using namespace std;
namespace org_restfulipc 
{

    enum class NotificationEvent : int
    {
            Created,
            Updated,
            Removed
    };


    class NotifierResource : public AbstractResource
    {
    public:
        typedef shared_ptr<NotifierResource> ptr;
        NotifierResource();

        virtual void handleRequest(int &socket, int matchedPathLength, const HttpMessage& request);
        void notifyClients(const char* event, const char* resourceIdentification);

    private:
        void addClient(int socket);

        vector<int> mClientSockets;
        mutex mMutex;
    };

}
#endif // NOTIFIERRESOURCE_H
