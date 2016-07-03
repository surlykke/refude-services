#ifndef POWER_APPLICATION_H
#define POWER_APPLICATION_H

#include <vector>
#include <map>
#include <QCoreApplication>
#include <ripc/json.h>
#include <ripc/service.h>
#include <ripc/notifierresource.h>
#include "batteryresource.h"
    
class QDBusInterface;
    
class PropertiesIF;
    
namespace org_restfulipc 
{

    class PowerApplication : public QCoreApplication
    {
    public:
        PowerApplication(Service& service, NotifierResource::ptr notifierResource, int& argc, char** argv);
        virtual ~PowerApplication();

    private:
        Service& service;
        NotifierResource::ptr notifierResource;

        std::map<PropertiesIF*, BatteryResource::ptr> batteries;

        Json buildJson(const QVariantMap& map);

    private slots:
        void onPropertiesChanged();

    };
}
#endif /* POWER_APPLICATION_H */

