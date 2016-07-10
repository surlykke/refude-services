#ifndef APPLICATIONSRESOURCE_H
#define APPLICATIONSRESOURCE_H
#include <ripc/localizedjsonresource.h>

namespace org_restfulipc
{
    class ApplicationsResource : public LocalizedJsonResource
    {
    public:
        typedef std::shared_ptr<ApplicationsResource> ptr; 
        ApplicationsResource();
        virtual ~ApplicationsResource();

        virtual void doPOST(int& socket, HttpMessage& request);
    };
}
#endif /* APPLICATIONSRESOURCE_H */

