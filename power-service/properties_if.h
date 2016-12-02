/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef PROPERTIES_IF_H
#define PROPERTIES_IF_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.freedesktop.DBus.Properties
 * Generated with qdbusxml2cpp
 */
class PropertiesIF: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.DBus.Properties"; }

public:
    PropertiesIF(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~PropertiesIF();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<QDBusVariant> Get(const QString &interface, const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(interface) << QVariant::fromValue(name);
        return asyncCallWithArgumentList(QStringLiteral("Get"), argumentList);
    }

    inline QDBusPendingReply<QVariantMap> GetAll(const QString &interface)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(interface);
        return asyncCallWithArgumentList(QStringLiteral("GetAll"), argumentList);
    }

    inline QDBusPendingReply<> Set(const QString &interface, const QString &name, const QDBusVariant &value)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(interface) << QVariant::fromValue(name) << QVariant::fromValue(value);
        return asyncCallWithArgumentList(QStringLiteral("Set"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void PropertiesChanged(const QString &interface, const QVariantMap &changed_properties, const QStringList &invalidated_properties);
};

#endif