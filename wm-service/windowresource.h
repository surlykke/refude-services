#ifndef WINDOWRESOURCE_H
#define WINDOWRESOURCE_H

#include "jsonresource.h"
#include "windowinfo.h"

namespace refude
{
    struct WindowResource : public JsonResource
    {
        WindowResource(Json&& action, Window window);
        virtual void doPOST(Descriptor& socket, HttpMessage& request, const char* remainingPath) override;
        Window window;
    };
}

#endif // WINDOWRESOURCE_H
