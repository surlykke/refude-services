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

    class MimetypeResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<MimetypeResource> ptr;
        MimetypeResource(Json&& rootJson, Map<Json>&& mimetypeJsons);
        virtual ~MimetypeResource();
        void setRoot(Json&& root, NotifierResource::ptr notifier);
        void setMimetypeJsons(Map<Json>&& mimetypeJsons, NotifierResource::ptr notifier);
        void doPATCH(int& socket, HttpMessage& request, const char* remainingPath) override;

    protected:
        Buffer buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers) override;

    private:
        bool match(const vector<const char*>& searchTerms, 
                   Json& mimetypeJson, 
                   const vector<string>& acceptableLocales);
        Json rootJson;
        Map<Json> mimetypeJsons;
    };

}
#endif /* MYMETYPERESOURCE_H */

