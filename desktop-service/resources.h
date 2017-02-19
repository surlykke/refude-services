#ifndef RESOURCES_H
#define RESOURCES_H

#include "jsonresource.h"

namespace refude
{
    struct ApplicationResource : public JsonResource
    {
        ApplicationResource(Json&& application);
        virtual void doPOST(Descriptor& socket, HttpMessage& request, const char* remainingPath) override;
    };

    struct MimetypeResource : public JsonResource
    {
        MimetypeResource(Json&& mimetype);
        virtual void doPATCH(Descriptor& socket, HttpMessage& request, const char* remainingPath) override;
    };

}

#endif // RESOURCES_H
