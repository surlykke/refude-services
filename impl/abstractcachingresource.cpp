#include "abstractcachingresource.h"

namespace org_restfulipc
{
    AbstractCachingResource::AbstractCachingResource() :
        AbstractResource(),
        cache()
    {
    }

    AbstractCachingResource::~AbstractCachingResource()
    {
    }

    void AbstractCachingResource::doGET(int& socket, HttpMessage& request, const char* remainingPath)
    {
        std::cout << "AbstractCachingResource::doGET\n";
        Buffer requestSignature = getSignature(request, remainingPath);
        if (cache.find(requestSignature.data()) < 0) {
            std::cout << "Not in cache, building...\n";
            map<string, string> additionalHeaders;
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
        Buffer& resp = cache[requestSignature.data()];
        std::cout << "Sending: " << resp.data() << "\n";
        sendFully(socket, resp.data(), resp.size());
    }

    Buffer AbstractCachingResource::getSignature(HttpMessage& request, const char* remainingPath)
    {
        static vector<const char*> interestingHeaders = {"accept-language"}; // FIXME: What else?

        Buffer result;
        request.queryParameterMap.each([&result](const char* key, const vector<const char*> values) {
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
        cache.clear();
    }

}