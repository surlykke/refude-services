#ifndef BATTERYRESOURCE_H
#define BATTERYRESOURCE_H
#include <QString>
#include <QObject>
#include <QVariant>
#include <ripc/genericresource.h>
#include <ripc/notifierresource.h>
#include "properties_if.h"

class QDBusInterface;

namespace org_restfulipc
{
    class ChangeWatcher;

    class BatteryResource : public GenericResource
    {
    public:
        typedef std::shared_ptr<BatteryResource> ptr;
        BatteryResource(NotifierResource::ptr notifier);
        virtual ~BatteryResource();
        void setJson(Json&& json);
        void setProps(const QVariantMap& props);
        Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers) override;

    private:
        NotifierResource::ptr notifier;
    };

    

}
#endif /* BATTERYRESOURCE_H */

