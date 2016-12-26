/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef CONNMANOBJECT_H
#define CONNMANOBJECT_H

#include <QMetaType>
#include <QVariantMap>
#include <QDBusMetaType>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusAbstractInterface>
#include <QDebug>
#include "json.h"

typedef QPair<QDBusObjectPath, QVariantMap> ObjectProperties;
typedef QList<ObjectProperties> ObjectPropertiesList;

Q_DECLARE_METATYPE(ObjectProperties)
Q_DECLARE_METATYPE(ObjectPropertiesList)

extern bool dbus_types_registered;

namespace refude
{

    class ConnmanObject : public QDBusAbstractInterface
    {
        Q_OBJECT

    public:
        ConnmanObject(const QString& path, const char* interface, const QVariantMap properties = QVariantMap());
        Json properties;

    private slots:
        void onPropertyChanged(const QString& name, const QDBusVariant& newValue);

    Q_SIGNALS:
       	void PropertyChanged(const QString& name, const QDBusVariant& newValue);
		void jsonChanged();
    };

    class ConnmanManager : public ConnmanObject
    {
        Q_OBJECT

    public:
        ConnmanManager();

    Q_SIGNALS:
        void TechnologyAdded(const QDBusObjectPath& object, const QVariantMap& properties);
        void TechnologyRemoved(const QDBusObjectPath& object);
        void ServicesChanged(ObjectPropertiesList added, const QList<QDBusObjectPath>& removed);
    };
}

#endif // CONNMANOBJECT_H
