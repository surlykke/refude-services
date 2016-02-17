#ifndef MIMERESOURCEBUILDER_H
#define MIMERESOURCEBUILDER_H

#include "json.h" 
#include "service.h"

namespace org_restfulipc
{
    class MimeResourceBuilder
    {
    public:
        MimeResourceBuilder(Service* service);
        virtual ~MimeResourceBuilder();
        
        void build(const char* xmlFilePath);
    
    private:
        Json& root();
        Json& type(const char* typeName);
        Json& subtype(const char* typeName, const char* subtype);
        Service* service;
    };
}
#endif /* MIMERESOURCEBUILDER_H */

