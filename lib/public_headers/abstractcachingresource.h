/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef ABSTRACTCACHINGRESOURCE_H
#define ABSTRACTCACHINGRESOURCE_H
#include <mutex>

#include "buffer.h"

#include "abstractresource.h"

namespace refude
{
    class AbstractCachingResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<AbstractCachingResource> ptr;
        AbstractCachingResource();
        virtual ~AbstractCachingResource();
        virtual void doGET(int& socket, HttpMessage& request) override;
        std::recursive_mutex m;

    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers) = 0;
        
        Buffer getSignature(HttpMessage& request);
        void clearCache();
    
    private:
        Map<Buffer> cache;
        
    };

}

#endif /* ABSTRACTCACHINGRESOURCE_H */

