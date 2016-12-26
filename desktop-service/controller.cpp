/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <set>
#include "server.h"
#include "jsonresource.h"
#include "jsonwriter.h"
#include "notifierresource.h"
#include "xdg.h"


#include "applicationcollector.h"
#include "mimetypecollector.h"
#include "mimeappslistreader.h"
#include "desktopwatcher.h"
#include "runapplication.h"
#include "fd.h"
#include "controller.h"
#include "typedefs.h"

namespace refude
{

    struct ApplicationsResource : public CollectionResource
    {
        ApplicationsResource() : CollectionResource("applicationId") {}
        void doPOST(Fd& socket, HttpMessage& request, Server* server) override
        {
            if (indexes.find(request.remainingPath) < 0) throw HttpCode::Http404;
            runApplication(jsonArray[indexes[request.remainingPath]]["Exec"].toString());
            throw HttpCode::Http204; // Hackish - I know.
        }

    };
   
    struct MimetypesResource : public CollectionResource
    {
        MimetypesResource() : CollectionResource("mimetype") {}
        void doPATCH(Fd& socket, HttpMessage& request, Server* server) override
        {
            if (indexes.find(request.remainingPath) < 0)  throw HttpCode::Http404;
            Json mergeJson;
            mergeJson << request.body;
            if (mergeJson.type() != JsonType::Object) throw HttpCode::Http406;
            if (mergeJson.size() != 1) throw HttpCode::Http422;
            
            if (!mergeJson.contains("defaultApplications")) throw HttpCode::Http422;
            if (mergeJson["defaultApplications"].type() != JsonType::Array) throw HttpCode::Http422;
            MimeappsList mimeappsList(xdg::config_home() + "/mimeapps.list");
            auto& defaultAppsForMime = mimeappsList.defaultApps[request.remainingPath];
            defaultAppsForMime.clear();
            mergeJson["defaultApplications"].eachElement([&defaultAppsForMime](Json& element) { 
                if (element.type() != JsonType::String) throw HttpCode::Http422;
                defaultAppsForMime.push_back(element.toString());
            });

            mimeappsList.write();
            throw HttpCode::Http204;
        }
    };


    Controller::Controller() : 
        service(),
        applicationsResource(std::make_shared<ApplicationsResource>()),
        mimetypesResource(std::make_shared<MimetypesResource>()),
        notifier(std::make_shared<NotifierResource>())
    {
        
        service.mapPrefix(applicationsResource, "/applications");
        service.mapPrefix(mimetypesResource, "/mimetypes");
        service.map(notifier, "/notify");
    }

    Controller::~Controller()
    {
    }

    void Controller::update()
    {
        ApplicationCollector applicationCollector;
        applicationCollector.collect();
       
        MimetypeCollector mimetypeCollector;
        mimetypeCollector.collect();
       
        mimetypeCollector.addAssociations(applicationCollector.jsonArray);
        mimetypeCollector.addDefaultApplications(applicationCollector.defaultApplications);

        CollectionResourceUpdater applicationsResourceUpdater(applicationsResource);
        applicationsResourceUpdater.update(applicationCollector.jsonArray);
        applicationsResourceUpdater.notify(notifier, "applications");

        CollectionResourceUpdater mimetypesResourceUpdater(mimetypesResource);
        mimetypesResourceUpdater.update(mimetypeCollector.jsonArray);
        mimetypesResourceUpdater.notify(notifier, "mimetypes");
    }

}
