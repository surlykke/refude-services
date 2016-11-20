/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <QDebug>
#include <QDBusReply>
#include "controller.h"
#include "agent.h"

Agent::Agent() : QObject()
{
}

void Agent::Release()
{
    // Nothing to do
}

void Agent::Cancel()
{
    qDebug() << "Canceled";
}

void Agent::ReportError(QDBusObjectPath servicePath, QString errorMessage)
{
    // FIXME
}

void Agent::ReportPeerError(QDBusObjectPath peerPath, QString errorMessage)
{
    // FIXME
}

void Agent::RequestBrowser(QDBusObjectPath servicePath, QString url)
{
    // FIXME
}

QVariantMap Agent::RequestInput(QDBusObjectPath servicePath, QVariantMap fields)
{
    return QVariantMap(); // FIXME
}

QVariantMap Agent::RequestPeerAuthorization(QDBusObjectPath peerPath, QVariantMap fields)
{
    return QVariantMap(); // FIXME
}

