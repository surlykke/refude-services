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
        PowerApplication(int& argc, char** argv);
        virtual ~PowerApplication();

        CollectionResource::ptr devicesResource;
        CollectionResource::ptr actionsResource;
        NotifierResource::ptr notifierResource;   
    
    public slots:
        void collectDeviceJsons();
        void collectActionJsons();

    private:
        std::vector<PropertiesIF*> deviceInterfaces;
        QDBusInterface* managerInterface;
    };
}
#endif /* POWER_APPLICATION_H */

