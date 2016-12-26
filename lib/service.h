#ifndef SERVICE_H
#define SERVICE_H

#include <memory>

#include "httpmessage.h"

namespace refude
{
    class Service
    {
    public:
        typedef std::shared_ptr<Service> ptr;
        Service();
        virtual ~Service();
        virtual void handle(int socket, HttpMessage::ptr request) = 0;
    };
}
#endif // SERVICE_H
