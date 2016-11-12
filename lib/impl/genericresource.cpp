/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <unistd.h>
#include <sys/socket.h>
#include "errorhandling.h"
#include "genericresource.h"

namespace refude
{

    GenericResource::GenericResource(const char* data, NotifierResource::ptr notifierResource) :
        AbstractCachingResource(),
        notifierResource(notifierResource),
        doc(data)
    {
    }

    GenericResource::~GenericResource()
    {
    }

    void GenericResource::update(const char* data)
    {
        doc = data;
        clearCache();
    }

    Buffer GenericResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        Buffer buf;
        buf.write(doc.data());
        return buf;
    }

}
