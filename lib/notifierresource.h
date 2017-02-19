/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef NOTIFIERRESOURCE_H
#define NOTIFIERRESOURCE_H

#include <mutex>
#include <vector>

#include "abstractresource.h"

namespace refude 
{
    struct NotificationStream
    {
        NotificationStream();
        void addClient(Descriptor&& clientSocket);
        void resourceRemoved(const std::string& path);
        void resourceAdded(const std::string& path);
        void resourceUpdated(const std::string& path);
        void notifyClients(const std::string& event,  const std::string& path);

    private:
        std::vector<Descriptor> mClientSockets;
        std::mutex mutex;

    };

    class NotifierResource : public AbstractResource
    {
    public:
        typedef std::unique_ptr<NotifierResource> ptr;
        NotifierResource(NotificationStream* notificationStream);

        virtual void doGET(Descriptor& socket, HttpMessage& request, const char* remainingPath) override;
    private:
        NotificationStream* notificationStream;
    };


}
#endif // NOTIFIERRESOURCE_H
