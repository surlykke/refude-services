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

        char* rootTemplate;
        char* typeTemplate;
        char* subtypeTemplate;
        char* readFile(const char* path);
    };

}

#endif // JSONBUILDER_H
