#ifndef MYMETYPERESOURCE_H
#define MYMETYPERESOURCE_H

#include "jsonresource.h"

namespace org_restfulipc 
{
class DesktopService;
class MimetypeResource : public LocalizedJsonResource
{
public:
    typedef std::shared_ptr<MimetypeResource> ptr;
    MimetypeResource();
    virtual ~MimetypeResource();
    virtual void doPatch(int socket, const HttpMessage& request);
    void setService(DesktopService* service);
private:
    DesktopService* service;
};

}
#endif /* MYMETYPERESOURCE_H */

