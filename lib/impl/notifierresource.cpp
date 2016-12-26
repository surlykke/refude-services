/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <sys/uio.h>
#include <unistd.h>
#include <asm-generic/socket.h>
#include <sys/socket.h>

#include "comm.h"
#include "errorhandling.h"

#include "notifierresource.h"

namespace refude
{
    typedef std::function<void()> NotificationTask;

    class NotificationLoop
    {
        friend class NotifierResource;

        static void run(NotificationTask&& task)
        {
            inst().notificationTasks.enqueue(std::move(task));
        }

        static NotificationLoop& inst()
        {
            static NotificationLoop notificationLoop;
            return notificationLoop;
        }

        NotificationLoop() :
            notificationTasks(),
            taskRunnerThread(&NotificationLoop::taskRunner, this)
        {
        }

        void taskRunner() {
            for (;;) {
                std::cout << "Notify loop - task\n";
                notificationTasks.dequeue()();
                std::cout << " -\n";
            }
        }

        Queue<NotificationTask> notificationTasks;
        std::thread taskRunnerThread;
    };





    NotifierResource::NotifierResource() :
        AbstractResource(),
        mClientSockets(),
        mMutex()
    {
    }

    void NotifierResource::handleRequest(Fd& socket, HttpMessage& request, Server* server)
    {
        /*NotificationLoop::run([this, socket]() {
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
            mClientSockets.push_back(socket);
        });

        socket = -1;*/
    }

    void NotifierResource::notify(const char* event, std::string data)
    {
        notify(event, data.data());
    }

    void NotifierResource::notify(const char* event, const char* data)
    {
        std::cout << "notify:" << event << ":" << data << "\n";
        /*NotificationLoop::run([this, event, data]() {
            char notification[300];

            int chunkLength = (int)strlen(event) + (int)strlen(data) + 14;
            int dataLength = snprintf(notification, 300, "%x\r\nevent:%s\ndata:%s\n\n\r\n", chunkLength,  event, data);
            if (dataLength >= 300) {
                throw RuntimeError("data too long");
            }

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
        });*/
    }
}
