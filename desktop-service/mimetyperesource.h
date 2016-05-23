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
#include <ripc/notifierresource.h>

namespace org_restfulipc
{
    class MimetypeResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<MimetypeResource> ptr;
        MimetypeResource(Map<Json>&& mimetypeJsons);
        virtual ~MimetypeResource();
        void setMimetypeJsons(Map<Json>&& mimetypeJsons, NotifierResource::ptr notifier);
        void doPATCH(int& socket, HttpMessage& request) override;

    protected:
        Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers) override;

    private:
        void add(const char* type, const char* subtype, Json& mimetypes);
        bool match(const std::vector<const char*>& searchTerms, 
                   Json& mimetypeJson, 
                   const std::vector<std::string>& acceptableLocales);
        
        Map<Json> mimetypeJsons;
    };

}
#endif /* MYMETYPERESOURCE_H */

