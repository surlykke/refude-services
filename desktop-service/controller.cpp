#include <set>
#include <ripc/service.h>
#include <ripc/jsonresource.h>
#include <ripc/notifierresource.h>

#include "applicationcollector.h"
#include "mimetypecollector.h"

#include "desktopwatcher.h"
#include "runapplication.h"

#include "controller.h"

namespace org_restfulipc
{

    struct ApplicationsResource : public CollectionResource
    {
        ApplicationsResource() : CollectionResource("applicationId") {}
        void doPOST(int& socket, HttpMessage& request) override
        {
            if (! indexes.contains(request.remainingPath)) throw HttpCode::Http404;
            runApplication(jsonArray[indexes[request.remainingPath]]["Exec"]);
            throw HttpCode::Http204;
        }

    };
   
    struct MimetypesResource : public CollectionResource
    {
        MimetypesResource() : CollectionResource("MimeType") {}
        void doPATCH(int& socket, HttpMessage& request) override
        {
            // FIXME
        }

    };


    Controller::Controller() : 
        service(),
        applicationsResource(std::make_shared<ApplicationsResource>()),
        mimetypesResource(std::make_shared<CollectionResource>("MimeType")),
        notifier(std::make_shared<NotifierResource>()),
        desktopWatcher(new DesktopWatcher(*this, true))        
    {
        
        service.map(applicationsResource, true, "applications");
        service.map(mimetypesResource, true, "mimetypes");
        service.map(notifier, true, "notify");
    }

    Controller::~Controller()
    {
    }

    void Controller::setupAndRun()
    {
        desktopWatcher->start();
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
