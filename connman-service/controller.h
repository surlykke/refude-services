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
#include "refude/collectionresource.h"
#include "refude/service.h"
#include "connmanobject.h"
#include "agent.h"

namespace refude
{

    class Controller : public QCoreApplication
    {
        Q_OBJECT
    public:
        Controller(int argc, char** argv);

    private:
        void updateTechnologiesJson();
        void updateTechnologyJson(const QString& path);
        void updateServicesJson();
        void updateServiceJson(const QString& path);

        ConnmanManager manager;
        QMap<QString, ConnmanObject*> technologyObjects;
        QMap<QString, ConnmanObject*> serviceObjects;
        QVector<QString> servicesOrder;
        Agent agent;
        CollectionResource::ptr technologiesResource;
        CollectionResource::ptr servicesResource;
        NotifierResource::ptr notifier;
        Service service;

    private slots:
        void onTechnologyAdded(const QDBusObjectPath& path, const QVariantMap& properties);
        void onTechnologyRemoved(const QDBusObjectPath& path);
        void onServicesUpdated(ObjectPropertiesList serviceObjects, const QList<QDBusObjectPath>& remove);
    };
}
#endif // CONTROLLER_H
