/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef AGENT_H
#define AGENT_H

#include <QDebug>
#include <QDBusContext>
#include <QDBusObjectPath>
class Controller;

class Agent : public QObject, public QDBusContext
{
    Q_OBJECT

public:
    explicit Agent();

    void Release();
    void Cancel();
    void ReportError(QDBusObjectPath serviceName, QString errorMessage);
    void ReportPeerError(QDBusObjectPath peerName, QString errorMessage);
    void RequestBrowser(QDBusObjectPath serviceName, QString url);
    QVariantMap RequestInput(QDBusObjectPath serviceName, QVariantMap fields);
    QVariantMap RequestPeerAuthorization(QDBusObjectPath peerName, QVariantMap fields);

    QMap<QString, QString> serviceNames;

signals:
    void operationCanceled();

};

#endif // AGENT_H
