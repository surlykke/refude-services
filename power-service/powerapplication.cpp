#include <iostream>
#include <string>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QDBusObjectPath>
#include <sys/socket.h>
#include <ripc/collectionresource.h>
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

   
    PowerApplication::PowerApplication(CollectionResource::ptr devicesResource, NotifierResource::ptr notifierResource, 
                                       int& argc, char** argv) : 
        QCoreApplication(argc, argv),
        devicesResource(devicesResource),
        notifierResource(notifierResource)
    {
        QDBusInterface* uPower = new QDBusInterface(UPOW_SERVICE, UPOW_PATH, UPOW_IF, sysBus);
        PropertiesIF* displayDeviceIF = 
            new PropertiesIF(UPOW_SERVICE, "/org/freedesktop/UPower/devices/DisplayDevice", sysBus);
        // Assuming that whenever anything changes, the displaydevice will change... (?)
        connect(displayDeviceIF, &PropertiesIF::PropertiesChanged, this, &PowerApplication::collectJsons);
        deviceInterfaces.push_back(displayDeviceIF);
        
        QDBusReply<QList<QDBusObjectPath>> reply = uPower->call("EnumerateDevices");
        foreach(QDBusObjectPath p, reply.value()) { 
            deviceInterfaces.push_back(new PropertiesIF(UPOW_SERVICE, p.path(), sysBus));
        }
    }

    PowerApplication::~PowerApplication()
    {
    }

    void PowerApplication::collectJsons()
    {
        Json deviceJsons = JsonConst::EmptyArray;
        for (PropertiesIF* device : deviceInterfaces) 
        {
            QVariantMap map =  device->GetAll(DEV_IF).value();
            Json jsonDevice = JsonConst::EmptyObject; 
            QString deviceId = device->path().mid(QString("/org/freedesktop/UPower/devices/").size());
            jsonDevice.append("deviceId", deviceId.toUtf8().constData());
            jsonDevice.append("NativePath", map["NativePath"].toString().toUtf8().constData());
            jsonDevice.append("Vendor", map["Vendor"].toString().toUtf8().constData());
            jsonDevice.append("Model", map["Model"].toString().toUtf8().constData());
            jsonDevice.append("Serial", map["Serial"].toString().toUtf8().constData());
            jsonDevice.append("UpdateTime", map["UpdateTime"].toDouble());
            jsonDevice.append("Type", map["Type"].toDouble());
            jsonDevice.append("PowerSupply", map["PowerSupply"].toBool());
            jsonDevice.append("HasHistory", map["HasHistory"].toBool());
            jsonDevice.append("HasStatistics", map["HasStatistics"].toBool());
            jsonDevice.append("Online", map["Online"].toBool());
            jsonDevice.append("Energy", map["Energy"].toDouble());
            jsonDevice.append("EnergyEmpty", map["EnergyEmpty"].toDouble());
            jsonDevice.append("EnergyFull", map["EnergyFull"].toDouble());
            jsonDevice.append("EnergyFullDesign", map["EnergyFullDesign"].toDouble());
            jsonDevice.append("EnergyRate", map["EnergyRate"].toDouble());
            jsonDevice.append("Voltage", map["Voltage"].toDouble());
            jsonDevice.append("TimeToEmpty", map["TimeToEmpty"].toDouble());
            jsonDevice.append("TimeToFull", map["TimeToFull"].toDouble());
            jsonDevice.append("Percentage", map["Percentage"].toDouble());
            jsonDevice.append("Temperature", map["Temperature"].toDouble());
            jsonDevice.append("IsPresent", map["IsPresent"].toBool());
            jsonDevice.append("State", map["State"].toDouble());
            jsonDevice.append("IsRechargeable", map["IsRechargeable"].toBool());
            jsonDevice.append("Capacity", map["Capacity"].toDouble());
            jsonDevice.append("Technology", map["Technology"].toDouble());
            jsonDevice.append("WarningLevel", map["WarningLevel"].toDouble());
            jsonDevice.append("IconName", map["IconName"].toString().toUtf8().constData());

            deviceJsons.append(std::move(jsonDevice));
        }
        
        CollectionResourceUpdater updater(devicesResource);
        updater.update(deviceJsons);
        updater.notify(notifierResource, "devices");
    }

}
