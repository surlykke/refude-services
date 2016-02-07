#ifndef JSONBUILDER_H
#define JSONBUILDER_H

#include <jsonresource.h>
#include <buffer.h>
#include <service.h>

namespace org_restfulipc
{
    class ResourceBuilder : public Service
    {
    public:
        ResourceBuilder(const char* mimedir);
        virtual ~ResourceBuilder();
    private:
        void readXml(const char* xmlFilePath);
        Json& root();
        Json& type(const char* typeName);
        Json& subtype(const char* typeName, const char* subtype);
    };

}

#endif // JSONBUILDER_H
