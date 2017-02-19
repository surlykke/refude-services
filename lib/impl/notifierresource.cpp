/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <shared_mutex>
#include "descriptor.h"
#include "errorhandling.h"
#include "comm.h"

#include "notifierresource.h"

namespace refude
{

    static const char* eventLine[] ={
        "event:created",
        "event:updated",
        "event:removed"
    };

    NotifierResource::NotifierResource(NotificationStream* notificationStream) :
        AbstractResource(),
        notificationStream(notificationStream)
    {
    }

    void NotifierResource::doGET(Descriptor& socket, HttpMessage& request, const char *remainingPath)
    {
        // TODO Validate request

        notificationStream->addClient(std::move(socket));
    }

    void NotificationStream::addClient(Descriptor&& clientSocket)
    {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (struct timeval));

        static const char* response =
                "HTTP/1.1 200 OK\r\n"
                "Connection: keep-alive\r\n"
                "Content-Type: text/event-stream\r\n"
                "Transfer-Encoding: chunked\r\n"
                "\r\n";
        static int responseLength = strlen(response);

        if (write(clientSocket, response, responseLength) == responseLength) {
            std::lock_guard<std::mutex> lock(mutex);
            mClientSockets.push_back(std::move(clientSocket));
        }
    }

    void NotificationStream::resourceRemoved(const std::string& path)
    {
        notifyClients("resource-removed", path);
    }

    void NotificationStream::resourceAdded(const std::string& path)
    {
        notifyClients("resource-added", path);
    }

    void NotificationStream::resourceUpdated(const std::string& path)
    {
        notifyClients("resource-updated", path);
    }


    void NotificationStream::notifyClients(const std::string& event, const std::string& data)
    {
        static const char* notificationTemplate =
                "%x\r\n"     // chunk length
                "event:%s\n" // event 
                "data:%s\n"  // data
                "\n"
                "\r\n";

        char notification[300];
        int chunkLength = event.size() + data.size() + 14;
        int dataLength = snprintf(notification,
                                  299,
                                  notificationTemplate,
                                  chunkLength,
                                  event.data(),
                                  data.data());

        if (dataLength >= 299) {
            throw RuntimeError("Notification too long");
        }

        std::lock_guard<std::mutex> lock(mutex);

        for (auto it = mClientSockets.begin(); it != mClientSockets.end();) {
            int bytesWritten = write(*it, notification, dataLength);
            if (bytesWritten < dataLength) {
                // We don't care if the non-full-write is due to an error or buffer full.
                // In the latter case, it's probably because the client isn't consuming.
                // Either way, we disconnect
                it = mClientSockets.erase(it);
            }
            else {
                it++;
            }
        }
    }


    NotificationStream::NotificationStream() :
        mClientSockets(),
        mutex()
    {
    }

}
