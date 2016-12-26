/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <iostream>
#include <string>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QDBusObjectPath>
#include <sys/socket.h>
#include "collectionresource.h"
#include "jsonwriter.h"
#include "jsonreader.h"
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

        void doPOST(Fd& socket, HttpMessage& request, Server* server) override
        {
            std::cout << "POST against " << request.path << ", remaining path: " << request.remainingPath << "\n";
            if (indexes.find(request.remainingPath) < 0) throw HttpCode::Http404;
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
            jsonDevice["deviceId"] = deviceId.toUtf8().constData();
            jsonDevice["NativePath"] = map["NativePath"].toString().toUtf8().constData();
            jsonDevice["Vendor"] = map["Vendor"].toString().toUtf8().constData();
            jsonDevice["Model"] = map["Model"].toString().toUtf8().constData();
            jsonDevice["Serial"] = map["Serial"].toString().toUtf8().constData();
            jsonDevice["UpdateTime"] = map["UpdateTime"].toDouble();
            jsonDevice["Type"] = deviceType(map["Type"].toInt());
            jsonDevice["PowerSupply"] = map["PowerSupply"].toBool() ? JsonConst::TRUE : JsonConst::FALSE;
            jsonDevice["HasHistory"] = map["HasHistory"].toBool() ? JsonConst::TRUE : JsonConst::FALSE;
            jsonDevice["HasStatistics"] = map["HasStatistics"].toBool() ? JsonConst::TRUE : JsonConst::FALSE;
            jsonDevice["Online"] = map["Online"].toBool() ? JsonConst::TRUE : JsonConst::FALSE;
            jsonDevice["Energy"] = map["Energy"].toDouble();
            jsonDevice["EnergyEmpty"] = map["EnergyEmpty"].toDouble();
            jsonDevice["EnergyFull"] = map["EnergyFull"].toDouble();
            jsonDevice["EnergyFullDesign"] = map["EnergyFullDesign"].toDouble();
            jsonDevice["EnergyRate"] = map["EnergyRate"].toDouble();
            jsonDevice["Voltage"] = map["Voltage"].toDouble();
            jsonDevice["TimeToEmpty"] = map["TimeToEmpty"].toDouble();
            jsonDevice["TimeToFull"] = map["TimeToFull"].toDouble();
            jsonDevice["Percentage"] = map["Percentage"].toDouble();
            jsonDevice["Temperature"] = map["Temperature"].toDouble();
            jsonDevice["IsPresent"] = map["IsPresent"].toBool() ? JsonConst::TRUE : JsonConst::FALSE;
            jsonDevice["State"] = deviceState(map["State"].toDouble());
            jsonDevice["IsRechargeable"] = map["IsRechargeable"].toBool() ? JsonConst::TRUE : JsonConst::FALSE;
            jsonDevice["Capacity"] = map["Capacity"].toDouble();
            jsonDevice["Technology"] = deviceTechnology(map["Technology"].toDouble());
            jsonDevice["WarningLevel"] = map["WarningLevel"].toDouble();
            jsonDevice["IconName"] = map["IconName"].toString().toUtf8().constData();

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
                managerInterface->call(QString("Can") + allActions[i]["actionId"].toString());

            if (canAction.value() == "yes") {
                availableActions.append(std::move(allActions[i]));
            }
        }

        CollectionResourceUpdater updater(actionsResource);
        updater.update(availableActions);
        updater.notify(notifierResource, "actions");
    }

}
