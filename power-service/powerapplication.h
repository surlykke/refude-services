/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef POWER_APPLICATION_H
#define POWER_APPLICATION_H

#include <QCoreApplication>
#include "notifierresource.h"
#include "jsonresourcecollection.h"
#include "service.h"
    
class QDBusInterface;
    
class PropertiesIF;
    
namespace refude 
{

    class PowerApplication : public QCoreApplication
    {
    public:
        PowerApplication(int& argc, char** argv);

    private slots:
        void collectDevices();
        
    private:
        void collectDeviceJsons();
        void collectActionJsons();

        Service service;
        NotifierResource::ptr notifier;
        JsonResourceCollection jsonResources;
        std::vector<PropertiesIF*> deviceInterfaces;
        QDBusInterface* managerInterface;
    };
}
#endif /* POWER_APPLICATION_H */

