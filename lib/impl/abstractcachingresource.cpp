/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include "httpprotocol.h"
#include "comm.h"
#include "abstractcachingresource.h"

namespace refude
{
    AbstractCachingResource::AbstractCachingResource() :
        AbstractResource(),
        cache()
    {
    }

    AbstractCachingResource::~AbstractCachingResource()
    {
    }

    void AbstractCachingResource::doGET(Fd& socket, HttpMessage& request, Server* server)
    {
        Buffer requestSignature = getSignature(request);
        if (cache.find(requestSignature.data()) < 0) {
            cache[requestSignature.data()]  = buildContent(request);
        }
        //return cache[requestSignature.data()];
    }

    Buffer AbstractCachingResource::getSignature(HttpMessage& request)
    {
        static std::vector<const char*> interestingHeaders = {"accept-language"}; // FIXME: What else?

        Buffer result;
        request.queryParameterMap.each([&result](const char* key, const std::vector<const char*> values) {
            for (const char* value : values)  {
                result.writeChr('&');
                result.writeStr(key);
                result.writeChr('=');
                result.writeStr(value);
            }
        });
        for (const char* headerName : interestingHeaders) {
            if (request.headers.find(headerName) > -1) {
                result.writeChr('@');
                result.writeStr(headerName);
                result.writeChr(':');
                result.writeStr(request.headers[headerName]);
            }
        }
        result.writeStr(request.path);
        return result;
    }

}
