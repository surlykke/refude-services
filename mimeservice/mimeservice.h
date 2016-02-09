#ifndef MIMESERVICE_H
#define MIMESERVICE_H

#include <jsonresource.h>
#include <buffer.h>
#include <service.h>

namespace org_restfulipc
{
    class MimeService : public Service
    {
    public:
        MimeService(const char* mimedir);
        virtual ~MimeService();
    private:
        void readXml(const char* xmlFilePath);
        Json& root();
        Json& type(const char* typeName);
        Json& subtype(const char* typeName, const char* subtype);
    };

}

#endif // MIMESERVICE_H
