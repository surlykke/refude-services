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
    private:
        void read(const char* xmlFilePath);
        void buildRoot();
        Json& buildType(const char* typeName);
        Json& buildSubtype(const char* typeName, const char* subtype);

        Json root;
    };

}

#endif // JSONBUILDER_H
