#include <iostream>
#include <string>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QDBusObjectPath>
#include <sys/socket.h>
#include <refude/collectionresource.h>
#include <refude/jsonwriter.h>
#include <refude/jsonreader.h>
#include "properties_if.h"

#include "powerapplication.h"

namespace refude
{

    struct ActionsResource : public CollectionResource 
    {
        ActionsResource(QDBusInterface* managerIf) : 
            CollectionResource("actionId"),
            managerIf(managerIf)
        {
        }

        void doPOST(int& socket, HttpMessage& request) override
        {
            std::cout << "POST against " << request.path << ", remaining path: " << request.remainingPath << "\n";
            if (!indexes.contains(request.remainingPath)) throw HttpCode::Http404;
            managerIf->call(request.remainingPath, false);
            throw HttpCode::Http204;
        }

        QDBusInterface* managerIf;
    };

    static const QDBusConnection sysBus = QDBusConnection::systemBus();
    static const char* const UPOW_SERVICE = "org.freedesktop.UPower";
    static const char* const UPOW_PATH = "/org/freedesktop/UPower";
    static const char* const UPOW_IF = "org.freedesktop.UPower";
    static const char* const DEV_IF = "org.freedesktop.UPower.Device";
    static const char* const DISPLAY_DEVICE_PATH = "/org/freedesktop/UPower/devices/DisplayDevice";
    static const char* const LOGIN1_SERVICE = "org.freedesktop.login1";
    static const char* const LOGIN1_PATH = "/org/freedesktop/login1";
    static const char* const LOGIN1_MANAGER_IF = "org.freedesktop.login1.Manager";
    
    QDBusInterface* makeIF(QString path, QString interface) {
        return new QDBusInterface(UPOW_SERVICE, path, interface, sysBus);
    }

   
    PowerApplication::PowerApplication(int& argc, char** argv) : 
        QCoreApplication(argc, argv)
 
    {
        managerInterface = new QDBusInterface(LOGIN1_SERVICE, LOGIN1_PATH, LOGIN1_MANAGER_IF, sysBus, this);
        actionsResource = std::make_shared<ActionsResource>(managerInterface);
        devicesResource = std::make_shared<CollectionResource>("deviceId");
        notifierResource = std::make_shared<NotifierResource>();
        
        QDBusInterface* uPower = new QDBusInterface(UPOW_SERVICE, UPOW_PATH, UPOW_IF, sysBus);
        PropertiesIF* displayDeviceIF = new PropertiesIF(UPOW_SERVICE, DISPLAY_DEVICE_PATH, sysBus);
        connect(displayDeviceIF, &PropertiesIF::PropertiesChanged, this, &PowerApplication::collectDeviceJsons);
        deviceInterfaces.push_back(displayDeviceIF);
        
        QDBusReply<QList<QDBusObjectPath>> reply = uPower->call("EnumerateDevices");
        foreach(QDBusObjectPath p, reply.value()) { 
            deviceInterfaces.push_back(new PropertiesIF(UPOW_SERVICE, p.path(), sysBus));
        }
    }

    PowerApplication::~PowerApplication()
    {
    }


    const char* deviceType(int index) {
        static const char* data[9] = {"Unknown", "Line Power", "Battery", "Ups", "Monitor", 
                                      "Mouse", "Keyboard", "Pda", "Phone"};
        if (index < 0 || index > 8) index = 0;
        return data[index];
    }
    
    const char* deviceState(int index) {
        static const char* data[7] = {"Unknown", "Charging", "Discharging", "Empty", 
                                      "Fully charged", "Pending charge", "Pending discharge"};
        if (index < 0 || index > 6) index = 0;
        return data[index];
    }

    const char* deviceTechnology(int index) {
        static const char* data[7] = {"Unknown", "Lithium ion", "Lithium polymer", "Lithium iron phosphate", 
                                      "Lead acid", "Nickel cadmium", "Nickel metal hydride"};
        if (index < 0 || index > 6) index = 0;
        return data[index];
    }

    void PowerApplication::collectDeviceJsons()
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
            jsonDevice.append("Type", deviceType(map["Type"].toInt()));
            jsonDevice.append("PowerSupply", map["PowerSupply"].toBool() ? JsonConst::TRUE : JsonConst::FALSE);
            jsonDevice.append("HasHistory", map["HasHistory"].toBool() ? JsonConst::TRUE : JsonConst::FALSE);
            jsonDevice.append("HasStatistics", map["HasStatistics"].toBool() ? JsonConst::TRUE : JsonConst::FALSE);
            jsonDevice.append("Online", map["Online"].toBool() ? JsonConst::TRUE : JsonConst::FALSE);
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
            jsonDevice.append("IsPresent", map["IsPresent"].toBool() ? JsonConst::TRUE : JsonConst::FALSE);
            jsonDevice.append("State", deviceState(map["State"].toDouble()));
            jsonDevice.append("IsRechargeable", map["IsRechargeable"].toBool() ? JsonConst::TRUE : JsonConst::FALSE);
            jsonDevice.append("Capacity", map["Capacity"].toDouble());
            jsonDevice.append("Technology", deviceTechnology(map["Technology"].toDouble()));
            jsonDevice.append("WarningLevel", map["WarningLevel"].toDouble());
            jsonDevice.append("IconName", map["IconName"].toString().toUtf8().constData());

            deviceJsons.append(std::move(jsonDevice));
        }
        
        CollectionResourceUpdater updater(devicesResource);
        updater.update(deviceJsons);
        updater.notify(notifierResource, "devices");
    }

    void PowerApplication::collectActionJsons()
    {
        const char* allActionsTemplate = R"json(
        [
            {
                "actionId": "PowerOff",
                "description": "Power off the machine",
                "icon": "system-shutdown"
            },
            {
                "actionId": "Reboot",
                "description": "Reboot the machine",
                "icon": "system-reboot"
            },
            {
                "actionId": "Suspend",
                "description": "Suspend the machine",
                "icon": "system-suspend"
            },
            {
                "actionId": "Hibernate",
                "description": "Put the machine into hibernation",
                "icon": "system-suspend-hibernate"
            },
            {
                "actionId": "HybridSleep",
                "description": "Put the machine into hybrid sleep",
                "icon": "system-suspend-hibernate"
            }
        ] 
        )json";

        Json allActions;
        allActions << allActionsTemplate;
        Json availableActions = JsonConst::EmptyArray;


        for (int i = 0; i < allActions.size(); i++) {
            QDBusReply<QString> canAction = 
                managerInterface->call(QString("Can") + (const char*)allActions[i]["actionId"]);

            if (canAction.value() == "yes") {
                availableActions.append(std::move(allActions[i]));
            }
        }

        CollectionResourceUpdater updater(actionsResource);
        updater.update(availableActions);
        updater.notify(notifierResource, "actions");
    }

}
