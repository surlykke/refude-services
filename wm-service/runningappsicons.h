/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef RUNNINGAPPSICONS_H
#define RUNNINGAPPSICONS_H
#include <vector>
#include <string>
#include <utility>
#include "map.h"
#include "webserver.h"
namespace refude
{
    struct SizePath
    {
        unsigned long size;
        std::string path;
    };

    typedef std::vector<SizePath> SizePathList;

    class RunningAppsIcons : public AbstractResource
    {
    public:
        typedef std::unique_ptr<RunningAppsIcons> ptr;
        RunningAppsIcons();
        virtual ~RunningAppsIcons();
        virtual void doGET(Descriptor& socket, HttpMessage& request, const char* remainingPath);
    
        void addIcon(const char* iconName, const std::vector<unsigned long>& icon);
    private:
        std::string rootDir; 
        Map<SizePathList> iconPaths;

        void writePng(const char* filePath, const std::vector<unsigned long>& data, unsigned long pos);
        
    };
}
#endif /* RUNNINGAPPSICONS_H */

