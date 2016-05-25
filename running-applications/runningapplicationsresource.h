/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#ifndef RUNNINGAPPLICATIONSRESOURCE_H
#define RUNNINGAPPLICATIONSRESOURCE_H

#include <ripc/abstractresource.h>

namespace org_restfulipc
{
    class RunningApplicationsResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<RunningApplicationsResource> ptr;
        RunningApplicationsResource();
        virtual ~RunningApplicationsResource();
        void doGET(int& socket, HttpMessage& request) override;
        void doPOST(int& socket, HttpMessage& request) override;

    private:
        /**
         * returns an array of jsons, each representing a running application
         */
    };
}
#endif /* RUNNINGAPPLICATIONSRESOURCE_H */

