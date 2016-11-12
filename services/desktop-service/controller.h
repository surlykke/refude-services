#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ripc/service.h>
#include <ripc/json.h>
#include <ripc/collectionresource.h>
#include <ripc/notifierresource.h>

namespace org_restfulipc
{
    class DesktopWatcher;

    class Controller
    {
    public:
        Controller();
        virtual ~Controller();
        void setupAndRun();
        Service service;
 
    private:
        void update();
        
        CollectionResource::ptr applicationsResource;
        CollectionResource::ptr mimetypesResource;
        NotifierResource::ptr notifier; 

        DesktopWatcher* desktopWatcher;
        
        friend class DesktopWatcher; 
    };
}
#endif /* CONTROLLER_H */

