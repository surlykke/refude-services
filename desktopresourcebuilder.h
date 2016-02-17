#ifndef DESKTOPRESOURCEBUILDER_H
#define DESKTOPRESOURCEBUILDER_H

#include <string>
#include <list>
#include "service.h"

namespace org_restfulipc 
{

    class DesktopResourceBuilder
    {
    public:
        DesktopResourceBuilder(Service* service);
        virtual ~DesktopResourceBuilder();
        void build();
    private:
        void build(std::string applicationsDir, std::string subDir);
        std::list<std::string> applicationsDirs;
   
        Service* service;
    };

}
#endif /* DESKTOPRESOURCEBUILDER_H */

