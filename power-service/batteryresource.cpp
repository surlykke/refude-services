#include <locale.h>
#include <QDBusInterface>
#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include "properties_if.h"

#include "batteryresource.h"

namespace org_restfulipc
{

    BatteryResource::BatteryResource(NotifierResource::ptr notifier) :
        notifier(notifier)
    {
    }


    BatteryResource::~BatteryResource()
    {
    }

    void BatteryResource::setJson(Json&& json)
    {
        update(JsonWriter(json).buffer.data());
        notifier->notifyClients("resource-updated", "/devices/battery_BAT0"); // FIXME
    }

    Buffer BatteryResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        headers["Link"] = "</notify>;rel=http://org.restfulipc.relations.Notify";
        return GenericResource::buildContent(request, headers);
    }


}
