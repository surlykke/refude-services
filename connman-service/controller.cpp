/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <memory>
#include <QDBusReply>
#include "json.h"
#include "jsonwriter.h"
#include "xdg.h"

#include "agentadaptor.h"
#include "controller.h"


namespace refude
{
    Controller::Controller(int argc, char** argv) :
        QCoreApplication(argc, argv),
        manager(),
        technologyObjects(),
        serviceObjects(),
        agent(),
        service(),
        notifier(std::make_unique<NotifierResource>()),
        resources(&service, notifier.get())
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
            technologyObjects[op.first.path()] = new DBusProxy(op.first.path(), "net.connman.Technology", op.second);
        }

        QDBusReply<ObjectPropertiesList> GetServicesReply = manager.call("GetServices");
        onServicesUpdated(GetServicesReply.value(), QList<QDBusObjectPath>());

        std::string socketpath = xdg::runtime_dir() + "/org.refude.connman-service";
        service.serve(socketpath.data());
    }

    void Controller::onTechnologyAdded(const QDBusObjectPath& path, const QVariantMap& properties)
    {
        if (!technologyObjects.contains(path.path())) {
            technologyObjects[path.path()] = new DBusProxy(path.path(), "net.connman.Technology", properties);
        }

        update();
    }


    void Controller::onTechnologyRemoved(const QDBusObjectPath& path)
    {
        if (technologyObjects.contains(path.path())) {
            technologyObjects.take(path.path())->deleteLater();
        }
        update();
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
                    serviceObjects[path] = new DBusProxy(path, "net.connman.Service", properties);
                    agent.serviceNames[path] = properties["Name"].toString();
                }
                else {
                    // This would be a hidden essid. We leave them out as we don't (yet) have
                    // functionality to handle them
                    continue;
                }
            }

            servicesOrder.push_back(path.toStdString());
        }

        update();
    }


    void Controller::updateTechnologyJson(const QString& path)
    {
        // FIXME
    }

    void Controller::updateServiceJson(const QString& path)
    {
        // FIXME
    }

    void Controller::update()
    {
        Json resources = JsonConst::EmptyArray;
        Json actions = JsonConst::EmptyArray;
        Map<JsonResource::ptr> path2Json;
        path2Json.beginInsert();

        for (const QString& qpath: technologyObjects.keys()) {
            std::string path = std::string("/device") + qpath.toStdString().data();
            std::string actionPath = path + "/toggle_enabled";
            path2Json.insert(path.data(),
                             std::make_unique<JsonResource>(technologyObjects[qpath]->properties.copy()));
            path2Json.insert(actionPath.data(),
                             std::make_unique<JsonResource>(JsonConst::EmptyObject)); // FIXME
            resources.append(path.data() + 1);
            actions.append(actionPath.data() + 1);
        }

        for (const std::string& serviceId : servicesOrder) {
            std::string path = std::string("/device") + serviceId;
            std::string actionPath = path + "/toggle_enabled";
            path2Json.insert(path.data(),
                             std::make_unique<JsonResource>(serviceObjects[serviceId.data()]->properties.copy()));
            path2Json.insert(actionPath.data(),
                             std::make_unique<JsonResource>(JsonConst::EmptyObject)); // FIXME
            resources.append(path.data() + 1);
            actions.append(actionPath.data() + 1);
        }

        path2Json.insert("/resources", std::make_unique<JsonResource>(std::move(resources)));
        path2Json.insert("/actions", std::make_unique<JsonResource>(std::move(actions)));
        path2Json.endInsert();
        this->resources.updateCollection(std::move(path2Json));
    }

}
