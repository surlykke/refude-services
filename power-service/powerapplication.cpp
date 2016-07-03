#include <iostream>
#include <string>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QDBusObjectPath>
#include <sys/socket.h>
#include "properties_if.h"

#include "powerapplication.h"

namespace org_restfulipc
{
    static const QDBusConnection sysBus = QDBusConnection::systemBus();
    static const char* const UPOW_SERVICE = "org.freedesktop.UPower";
    static const char* const UPOW_PATH = "/org/freedesktop/UPower";
    static const char* const UPOW_IF = "org.freedesktop.UPower";
    static const char* const DEV_IF = "org.freedesktop.UPower.Device";

    QDBusInterface* makeIF(QString path, QString interface) {
        return new QDBusInterface(UPOW_SERVICE, path, interface, sysBus);
    }

   
    PowerApplication::PowerApplication(Service& service, NotifierResource::ptr notifierResource, int& argc, char** argv) : 
        QCoreApplication(argc, argv),
        service(service),
        notifierResource(notifierResource)
    {
        QDBusInterface* uPower = new QDBusInterface(UPOW_SERVICE, UPOW_PATH, UPOW_IF, sysBus);
        QDBusReply<QList<QDBusObjectPath>> reply = uPower->call("EnumerateDevices");
        foreach(QDBusObjectPath p, reply.value()) { 
            PropertiesIF* propertiesIF = new PropertiesIF(UPOW_SERVICE, p.path(), sysBus);
            QVariantMap props =  propertiesIF->GetAll(DEV_IF).value();
            if ( props["Type"].toInt() == 2) {
                QString mappingPath = p.path().mid(strlen(UPOW_PATH));
                BatteryResource::ptr batRes = std::make_shared<BatteryResource>(notifierResource);
                batRes->setJson(buildJson(props));
                batteries[propertiesIF] = batRes;
                connect(propertiesIF, &PropertiesIF::PropertiesChanged, this, &PowerApplication::onPropertiesChanged);
                service.map(mappingPath.toUtf8().data(), batRes);
            } 
            else {
                propertiesIF->deleteLater();
            }
        }
    }

    PowerApplication::~PowerApplication()
    {
    }

    Json PowerApplication::buildJson(const QVariantMap& map)
    {
        Json json = JsonConst::EmptyObject;
        if (map["Type"].toInt() == 2) {
            json.append("NativePath", map["NativePath"].toString().toUtf8().constData());
            json.append("Vendor", map["Vendor"].toString().toUtf8().constData());
            json.append("Model", map["Model"].toString().toUtf8().constData());
            json.append("Serial", map["Serial"].toString().toUtf8().constData());
            json.append("UpdateTime", map["UpdateTime"].toDouble());
            json.append("Type", map["Type"].toDouble());
            json.append("PowerSupply", map["PowerSupply"].toBool());
            json.append("HasHistory", map["HasHistory"].toBool());
            json.append("HasStatistics", map["HasStatistics"].toBool());
            json.append("Online", map["Online"].toBool());
            json.append("Energy", map["Energy"].toDouble());
            json.append("EnergyEmpty", map["EnergyEmpty"].toDouble());
            json.append("EnergyFull", map["EnergyFull"].toDouble());
            json.append("EnergyFullDesign", map["EnergyFullDesign"].toDouble());
            json.append("EnergyRate", map["EnergyRate"].toDouble());
            json.append("Voltage", map["Voltage"].toDouble());
            json.append("TimeToEmpty", map["TimeToEmpty"].toDouble());
            json.append("TimeToFull", map["TimeToFull"].toDouble());
            json.append("Percentage", map["Percentage"].toDouble());
            json.append("Temperature", map["Temperature"].toDouble());
            json.append("IsPresent", map["IsPresent"].toBool());
            json.append("State", map["State"].toDouble());
            json.append("IsRechargeable", map["IsRechargeable"].toBool());
            json.append("Capacity", map["Capacity"].toDouble());
            json.append("Technology", map["Technology"].toDouble());
            json.append("WarningLevel", map["WarningLevel"].toDouble());
            json.append("IconName", map["IconName"].toString().toUtf8().constData());
        }

        return json;
    }

    void PowerApplication::onPropertiesChanged()
    {
        PropertiesIF* propsIF = (PropertiesIF*) sender();
        if (batteries.find(propsIF) != batteries.end()) {
            QVariantMap props = propsIF->GetAll(DEV_IF).value();
            batteries[propsIF]->setJson(buildJson(props));
        }
    }


}
