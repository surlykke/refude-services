#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <refude/service.h>
#include <refude/json.h>
#include <refude/collectionresource.h>
#include <refude/notifierresource.h>

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

