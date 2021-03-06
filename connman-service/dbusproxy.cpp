/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include "dbusproxy.h"

bool dbus_types_registered = []() -> bool {
    qDBusRegisterMetaType<ObjectProperties>();
    qDBusRegisterMetaType<ObjectPropertiesList>();
    return true;
}();

namespace refude
{
    Json qStringList2Json(const QStringList& list)
    {
        Json arr = JsonConst::EmptyArray;
        for (const QString& string : list) {
            arr.append(string.toUtf8().data());
        }
        return arr;
    }


    Json qVariant2Json(const QVariant& variant);

    Json qVariantMap2Json(const QVariantMap& variantMap)
    {
        std::vector<Map<Json>::Entry> pairs;
        for (const QString& key: variantMap.keys()) {
            Json json = qVariant2Json(variantMap[key]);
            if (json.undefined()) {
                continue;
            }
            pairs.push_back(Map<Json>::Entry(key.toUtf8().data(), std::move(json)));
        }
        Json map = JsonConst::EmptyObject;
        map.append(std::move(pairs));
        return map;
    }

    Json qVariant2Json(const QVariant& variant)
    {
        switch (variant.type())  {
        case QMetaType::Bool: return variant.toBool() ? JsonConst::TRUE : JsonConst::FALSE;
        case QMetaType::UChar:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Long:
        case QMetaType::Double: return variant.toDouble();
        case QMetaType::QChar:
        case QMetaType::QString: return variant.toString().toUtf8().data();
        case QMetaType::QStringList: return qStringList2Json(variant.toStringList());
        case QMetaType::QVariantMap: return qVariantMap2Json(variant.toMap());
        default:
            if ((int)variant.type() == qMetaTypeId<ObjectProperties>()) {
                return qVariantMap2Json(variant.value<ObjectProperties>().second);
            }
            else {
                std::cerr << "Warning, cannont convert " << variant.typeName() << " to json\n";
                return Json();
            }
        }
    }

    DBusProxy::DBusProxy(const QString& path, const char* interface, const QVariantMap properties) :
        QDBusAbstractInterface("net.connman", path, interface, QDBusConnection::systemBus(), 0),
        properties(qVariant2Json(properties))
    {
        this->properties["path"] = path.toUtf8().data() + 1; // Skip leading '/'
        connect(this, &DBusProxy::PropertyChanged, this, &DBusProxy::onPropertyChanged);
    }

    void DBusProxy::onPropertyChanged(const QString& name, const QDBusVariant& newValue)
    {
        properties[name.toStdString()] = qVariant2Json(newValue.variant());
		emit jsonChanged();
    }

    ConnmanManager::ConnmanManager() : DBusProxy("/", "net.connman.Manager")
    {
        qDebug() << "Manager constructed..";
    }

}
