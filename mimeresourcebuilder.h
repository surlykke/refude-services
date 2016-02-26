#ifndef MIMERESOURCEBUILDER_H
#define MIMERESOURCEBUILDER_H
#include <string>
#include <map>
#include <vector>
#include "json.h" 
#include "service.h"
#include "typedefs.h"

namespace org_restfulipc
{
    using namespace std;
    class MimeResourceBuilder
    {
    public:
        MimeResourceBuilder(Service* service);
        virtual ~MimeResourceBuilder();
        
        void build(const char* xmlFilePath, MimeAppMap& associations, MimeAppMap& defaults);
        
    private:
        map<string, vector<string> > typesSubtypes;
        Json root(vector<string> types);
        Json type(string typeName, vector<string> subtypes);
        Json subtype(const char* typeName, const char* subtype);
        Service* service;
    };
}
#endif /* MIMERESOURCEBUILDER_H */

