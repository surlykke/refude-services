#include <set>
#include <ripc/service.h>
#include <ripc/jsonresource.h>
#include <ripc/notifierresource.h>

#include "applicationcollector.h"
#include "mimetypecollector.h"

#include "desktopwatcher.h"

#include "controller.h"
namespace org_restfulipc
{
    Controller::Controller() : 
        service(),
        applicationsResource(std::make_shared<CollectionResource>()),
        mimetypesResource(std::make_shared<CollectionResource>()),
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
        applicationsResourceUpdater.update(applicationCollector.jsonArray, "applicationId");
        for (std::string appId : applicationsResourceUpdater.addedResources) {
            notifier->resourceAdded("applications", appId.data());
        }
        for (std::string appId : applicationsResourceUpdater.removedResources) {
            notifier->resourceRemoved("applications", appId.data());
        }
        for (std::string appId : applicationsResourceUpdater.updatedResources) {
            notifier->resourceUpdated("applications", appId.data());
        }

        CollectionResourceUpdater mimetypesResourceUpdater(mimetypesResource);
        mimetypesResourceUpdater.update(mimetypeCollector.jsonArray, "MimeType");
        for (std::string appId : mimetypesResourceUpdater.addedResources) {
            notifier->resourceAdded("mimetypes", appId.data());
        }
        for (std::string appId : mimetypesResourceUpdater.removedResources) {
            notifier->resourceRemoved("mimetypes", appId.data());
        }
        for (std::string appId : mimetypesResourceUpdater.updatedResources) {
            notifier->resourceUpdated("mimetypes", appId.data());
        }
    }
    
}
