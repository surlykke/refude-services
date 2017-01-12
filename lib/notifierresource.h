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

    class NotifierResource : public AbstractResource
    {
    public:
        typedef std::unique_ptr<NotifierResource> ptr;
        NotifierResource();

        virtual void doGET(int& socket, HttpMessage& request) override;

        void resourceRemoved(const std::string& p1,
                             const std::string& p2 = std::string(),
                             const std::string& p3 = std::string());

        void resourceAdded(const std::string& p1,
                           const std::string& p2 = std::string(),
                           const std::string& p3 = std::string());

        void resourceUpdated(const std::string& p1,
                             const std::string& p2 = std::string(),
                             const std::string& p3 = std::string());

        void notifyClients(const std::string& event,  const std::string& path);

    private:
        void addClient(int socket);

        std::vector<int> mClientSockets;
        std::mutex mMutex;
    };

}
#endif // NOTIFIERRESOURCE_H
