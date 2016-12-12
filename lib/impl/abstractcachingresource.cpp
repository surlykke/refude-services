/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

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

    void AbstractCachingResource::doGET(int& socket, HttpMessage& request)
    {
        std::lock_guard<std::recursive_mutex> lock(m);
    
        Buffer requestSignature = getSignature(request);
        if (cache.find(requestSignature.data()) < 0) {
            std::map<std::string, std::string> additionalHeaders;
            Buffer content = buildContent(request, additionalHeaders);
            Buffer& response = cache[requestSignature.data()];
            response.write("HTTP/1.1 200 OK\r\n"
                           "Content-Type: application/json; charset=UTF-8\r\n");
            for (auto headerPair : additionalHeaders) {
                response.write(headerPair.first.data());
                response.write(":");
                response.write(headerPair.second.data());
                response.write("\r\n");
            }
            response.write("content-length: ");
            response.write(content.size());
            response.write("\r\n\r\n");
            response.write(content.data());
        } 
        Buffer& response = cache[requestSignature.data()];
        sendFully(socket, response.data(), response.size());
    }

    Buffer AbstractCachingResource::getSignature(HttpMessage& request)
    {
        static std::vector<const char*> interestingHeaders = {"accept-language"}; // FIXME: What else?

        Buffer result;
        request.queryParameterMap.each([&result](const char* key, const std::vector<const char*> values) {
            for (const char* value : values)  {
                result.write('&');
                result.write(key);
                result.write('=');
                result.write(value);
            }
        });
        for (const char* headerName : interestingHeaders) {
            if (request.headers.find(headerName) > -1) {
                result.write('@');
                result.write(headerName);
                result.write(':');
                result.write(request.headers[headerName]);
            }
        }
        result.write(request.path);
        return result;
    }

    void AbstractCachingResource::clearCache()
    {
        std::lock_guard<std::recursive_mutex> lock(m);
        cache.clear();
    }

}
