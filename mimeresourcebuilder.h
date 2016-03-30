#ifndef MIMERESOURCEBUILDER_H
#define MIMERESOURCEBUILDER_H
#include <ripc/json.h>
#include <ripc/service.h>

#include "typedefs.h"
#include "mimetyperesource.h"

namespace org_restfulipc
{
    using namespace std;
    class MimeResourceBuilder
    {
    public:
        MimeResourceBuilder();
        virtual ~MimeResourceBuilder();
        JsonResource::ptr rootResource;
        map<string, MimetypeResource::ptr> mimetypeResources;
    private:
        void build();
    };
}
#endif /* MIMERESOURCEBUILDER_H */

