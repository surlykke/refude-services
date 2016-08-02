#ifndef POWER_APPLICATION_H
#define POWER_APPLICATION_H

#include <vector>
#include <map>
#include <QCoreApplication>
#include <ripc/json.h>
#include <ripc/service.h>
#include <ripc/notifierresource.h>
#include <ripc/jsonresource.h>
    
class QDBusInterface;
    
class PropertiesIF;
    
namespace org_restfulipc 
{

    class PowerApplication : public QCoreApplication
    {
    public:
        PowerApplication(CollectionResource::ptr devicesResource, NotifierResource::ptr notifierResource, 
                         int& argc, char** argv);
        virtual ~PowerApplication();

    public slots:
        void collectJsons();

    private:
        CollectionResource::ptr devicesResource;
        NotifierResource::ptr notifierResource;
        std::vector<PropertiesIF*> deviceInterfaces;

    };
}
#endif /* POWER_APPLICATION_H */

