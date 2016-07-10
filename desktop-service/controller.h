#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ripc/service.h>
#include <ripc/json.h>


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
        void updateApplicationHandersResources(Json& applicationJsons);
        void updateResources(Json& applicationJsons, std::string prefix);
        void updateMimetypesResources(Json& newMimetypes);

        template<class C>
        std::shared_ptr<C> ptr(const char* path) 
        {
            return std::dynamic_pointer_cast<C>(service.mapping(path));
        }

        DesktopWatcher* desktopWatcher;
        
        friend class DesktopWatcher; 
    };
}
#endif /* CONTROLLER_H */

