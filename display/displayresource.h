/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   displayresource.h
 * Author: Christian Surlykke
 *
 * Created on 28. maj 2016, 23:20
 */

#ifndef DISPLAYRESOURCE_H
#define DISPLAYRESOURCE_H

#include <ripc/abstractcachingresource.h>

namespace org_restfulipc
{
    class DisplayResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<DisplayResource> ptr;
        DisplayResource();
        virtual ~DisplayResource();
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers);

    };
}
#endif /* DISPLAYRESOURCE_H */

