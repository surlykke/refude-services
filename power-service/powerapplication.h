/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef POWER_APPLICATION_H
#define POWER_APPLICATION_H

#include <vector>
#include <map>
#include <QCoreApplication>
#include "json.h"
#include "server.h"
#include "notifierresource.h"
#include "jsonresource.h"
    
class QDBusInterface;
    
class PropertiesIF;
    
namespace refude 
{

    class PowerApplication : public QCoreApplication
    {
    public:
        PowerApplication(int& argc, char** argv);
        virtual ~PowerApplication();

        CollectionResource::ptr devicesResource;
        CollectionResource::ptr actionsResource;
        NotifierResource::ptr notifierResource;   
    
    public slots:
        void collectDeviceJsons();
        void collectActionJsons();

    private:
        std::vector<PropertiesIF*> deviceInterfaces;
        QDBusInterface* managerInterface;
    };
}
#endif /* POWER_APPLICATION_H */

