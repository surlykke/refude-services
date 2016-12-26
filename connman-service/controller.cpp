/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <QDBusReply>
#include "json.h"
#include "jsonwriter.h"
#include "xdg.h"

#include "agentadaptor.h"
#include "controller.h"

/**
 * @brief convertFully traverses down the map to find DBusArguments and convert them to QVariants
 * @param map
 * @return
 */

namespace refude
{
    Controller::Controller(int argc, char** argv) :
        QCoreApplication(argc, argv),
        manager(),
        technologyObjects(),
        serviceObjects(),
        agent(),
        technologiesResource(std::make_shared<CollectionResource>("path")),
        servicesResource(std::make_shared<CollectionResource>("path")),
        notifier(std::make_shared<NotifierResource>()),
        service()
    {
        QDBusObjectPath agentPath("/org/lxqt/lxqt_connman_agent");
        new AgentAdaptor(&agent);
        QDBusConnection::systemBus().registerObject(agentPath.path(), &agent);
        manager.call("RegisterAgent", QVariant::fromValue(agentPath));

        connect(&manager,
                SIGNAL(TechnologyAdded(const QDBusObjectPath&, const QVariantMap&)),
                this,
                SLOT(onTechnologyAdded(const QDBusObjectPath&, const QVariantMap&)));

        connect(&manager,
                SIGNAL(TechnologyRemoved(const QDBusObjectPath&)),
                this,
                SLOT(onTechnologyRemoved(const QDBusObjectPath&)));

        connect(&manager,
                SIGNAL(ServicesChanged(ObjectPropertiesList, const QList<QDBusObjectPath>&)),
                this,
                SLOT(onServicesUpdated(const ObjectPropertiesList&,  const QList<QDBusObjectPath>&)));



        QDBusReply<ObjectPropertiesList> GetTechnologiesReply = manager.call("GetTechnologies");;
        for (ObjectProperties op : GetTechnologiesReply.value()) {
            technologyObjects[op.first.path()] = new ConnmanObject(op.first.path(), "net.connman.Technology", op.second);
        }
        updateTechnologiesJson();

        QDBusReply<ObjectPropertiesList> GetServicesReply = manager.call("GetServices");
        onServicesUpdated(GetServicesReply.value(), QList<QDBusObjectPath>());

        service.mapPrefix(technologiesResource, "/technologies");
        service.mapPrefix(servicesResource, "/services");
        service.map(notifier, "/notify");

        std::string socketpath = xdg::runtime_dir() + "/org.refude.connman-service";
        service.serve(socketpath.data());
    }

    void Controller::onTechnologyAdded(const QDBusObjectPath& path, const QVariantMap& properties)
    {
        if (!technologyObjects.contains(path.path())) {
            technologyObjects[path.path()] = new ConnmanObject(path.path(), "net.connman.Technology", properties);
        }
        updateTechnologiesJson();
    }


    void Controller::onTechnologyRemoved(const QDBusObjectPath& path)
    {
        if (technologyObjects.contains(path.path())) {
            technologyObjects.take(path.path())->deleteLater();
        }
        updateTechnologiesJson();
    }

    void Controller::onServicesUpdated(ObjectPropertiesList services, const QList<QDBusObjectPath>& removed)
    {
        for (const auto& path : removed) {
            if (serviceObjects.contains(path.path())) {
                serviceObjects.take(path.path())->deleteLater();
            }
        }

        servicesOrder.clear();
        for (const auto& op : services) {
            QString path = op.first.path();
            QVariantMap properties = op.second;

            if (!serviceObjects.contains(path)) {
                if (properties.contains("Name") && !properties["Name"].toString().isEmpty()) {
                    serviceObjects[path] = new ConnmanObject(path, "net.connman.Service", properties);
                    agent.serviceNames[path] = properties["Name"].toString();
                }
                else {
                    // This would be a hidden essid. We leave them out as we don't (yet) have
                    // functionality to handle them
                    continue;
                }
            }

            servicesOrder.append(path);
        }

        updateServicesJson();
    }

    void Controller::updateTechnologiesJson()
    {
        Json technologiesJson = JsonConst::EmptyArray;
        for (const QString& path : technologyObjects.keys()) {
            technologiesJson.append(technologyObjects[path]->properties.copy());
        }
        std::cout << "updateTechnologies:\n" << JsonWriter(technologiesJson).buffer.data() << "\n";

        CollectionResourceUpdater technologiesResourceUpdater(technologiesResource);
        technologiesResourceUpdater.update(technologiesJson);
        technologiesResourceUpdater.notify(notifier, "technologies");
    }

    void Controller::updateTechnologyJson(const QString& path)
    {
        if (technologyObjects.contains(path)) {
            CollectionResourceUpdater technologiesResourceUpdater(technologiesResource);
            technologiesResourceUpdater.update(technologyObjects[path]->properties);
            technologiesResourceUpdater.notify(notifier, "technologies");
        }
    }

    void Controller::updateServicesJson()
    {
        Json servicesJson = JsonConst::EmptyArray;
        for (const QString& path : servicesOrder) {
            servicesJson.append(serviceObjects[path]->properties.copy());
        }

        std::cout << "services:\n" << JsonWriter(servicesJson).buffer.data() << "\n";
        CollectionResourceUpdater servicesResourceUpdater(servicesResource);
        servicesResourceUpdater.update(servicesJson);
        servicesResourceUpdater.notify(notifier, "services");
    }

    void Controller::updateServiceJson(const QString& path)
    {
        if (serviceObjects.contains(path)) {
            CollectionResourceUpdater servicesResourceUpdater(servicesResource);
            servicesResourceUpdater.updateSingle(serviceObjects[path]->properties);
            servicesResourceUpdater.notify(notifier, "services");
        }
    }
}
