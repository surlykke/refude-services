/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Restful Inter Process Communication (Ripc) project. 
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#include <sys/uio.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>

#include "notifierresource.h"
#include "errorhandling.h"

namespace refude
{

    static const char* eventLine[] ={
        "event:created",
        "event:updated",
        "event:removed"
    };

    NotifierResource::NotifierResource() :
        AbstractResource(),
        mClientSockets(),
        mMutex()
    {
    }

    void NotifierResource::doGET(int& socket, HttpMessage& request)
    {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof (struct timeval));

        static const char* response =
                "HTTP/1.1 200 OK\r\n"
                "Connection: keep-alive\r\n"
                "Content-Type: text/event-stream\r\n"
                "Transfer-Encoding: chunked\r\n"
                "\r\n";
        static int responseLength = strlen(response);


        sendFully(socket, response, responseLength);
        addClient(socket);
        socket = -1;
    }

    void NotifierResource::addClient(int socket)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mClientSockets.push_back(socket);
    }

    void NotifierResource::resourceAdded(const char* path)
    {
        notifyClients("resource-added", path);
    }

    void NotifierResource::resourceAdded(const char* p1, const char* p2)
    {
        resourceAdded((std::string(p1) + "/" + p2).data());
    }

    void NotifierResource::resourceAdded(const char* p1, const char* p2, const char* p3)
    {
        resourceAdded((std::string(p1) + "/" + p2 + "/" + p3).data());
    }

    void NotifierResource::resourceRemoved(const char* path)
    {
        notifyClients("resource-removed", path);
    }

    void NotifierResource::resourceRemoved(const char* p1, const char* p2)
    {
        resourceRemoved((std::string(p1) + "/" + p2).data());
    }

    void NotifierResource::resourceRemoved(const char* p1, const char* p2, const char* p3)
    {
        resourceRemoved((std::string(p1) + "/" + p2 + "/" + p3).data());
    }
 
    void NotifierResource::resourceUpdated(const char* path)
    {
        notifyClients("resource-updated", path);
    }

    void NotifierResource::resourceUpdated(const char* p1, const char* p2)
    {
        resourceUpdated((std::string(p1) + "/" + p2).data());
    }

    void NotifierResource::resourceUpdated(const char* p1, const char* p2, const char* p3)
    {
        resourceUpdated((std::string(p1) + "/" + p2 + "/" + p3).data());
    }
   
    void NotifierResource::notifyClients(const char* event, const char* data)
    {
        static const char* notificationTemplate =
                "%x\r\n"     // chunk length
                "event:%s\n" // event 
                "data:%s\n"  // data
                "\n"
                "\r\n";

        char notification[300];

        if (strlen(data) > 256) {
            throw RuntimeError("Path too long");
        }

        int chunkLength = strlen(event) + strlen(data) + 14;
        int dataLength = snprintf(notification,
                                  256,
                                  notificationTemplate,
                                  chunkLength,
                                  event,
                                  data);

        {
            std::lock_guard<std::mutex> lock(mMutex);

            for (auto it = mClientSockets.begin(); it != mClientSockets.end(); it++) {
                try {
                    sendFully(*it, notification, dataLength);
                }
                catch (C_Error c_error) {
                    // We close both if client closes and if any error occurs.
                    while (close(*it) < 0 && errno == EINTR);
                    *it = -1;
                }
            }

            // Reap the closed and compactify
            for (auto it = mClientSockets.begin(); it != mClientSockets.end();) {
                if (*it == -1) {
                    it = mClientSockets.erase(it);
                }
                else {
                    it++;
                }
            }
        }
    }
}
