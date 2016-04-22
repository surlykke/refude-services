/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef MYMETYPERESOURCE_H
#define MYMETYPERESOURCE_H

#include <ripc/jsonresource.h>

namespace org_restfulipc 
{
class DesktopService;
class MimetypeResource : public LocalizedJsonResource
{
public:
    typedef std::shared_ptr<MimetypeResource> ptr;
    MimetypeResource();
    virtual ~MimetypeResource();
    virtual void doPatch(int socket, HttpMessage& request);
private:
    DesktopService* service;
};

}
#endif /* MYMETYPERESOURCE_H */

