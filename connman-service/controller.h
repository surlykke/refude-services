/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QCoreApplication>
#include "jsonresourcecollection.h"
#include "service.h"
#include "dbusproxy.h"
#include "agent.h"

namespace refude
{

    class Controller : public QCoreApplication
    {
        Q_OBJECT
    public:
        Controller(int argc, char** argv);

    private:
        void updateTechnologyJson(const QString& path);
        void updateServiceJson(const QString& path);
        void update();

        ConnmanManager manager;
        QMap<QString, DBusProxy*> technologyObjects;
        QMap<QString, DBusProxy*> serviceObjects;

        std::vector<std::string> servicesOrder;

        Agent agent;
        Service service;
        NotifierResource::ptr notifier;
        JsonResourceCollection resources;

    private slots:
        void onTechnologyAdded(const QDBusObjectPath& path, const QVariantMap& properties);
        void onTechnologyRemoved(const QDBusObjectPath& path);
        void onServicesUpdated(ObjectPropertiesList serviceObjects, const QList<QDBusObjectPath>& remove);
    };
}
#endif // CONTROLLER_H
