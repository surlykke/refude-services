/* 
 * File:   resourcemap.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 23. juni 2015, 07:40
 */

#ifndef RESOURCEMAP_H
#define    RESOURCEMAP_H

#include <shared_mutex>

#include "resourcemap.h"
#include "abstractresource.h"

namespace org_restfulipc
{
    struct ResourceMappings;

    class ResourceMap
    {
    public:
        ResourceMap();
        virtual ~ResourceMap();

        void map(const char* path, AbstractResource* resource);
        void unMap(AbstractResource* resource);
        AbstractResource* resource(const char* path);

    private:
        std::shared_timed_mutex m;
        ResourceMappings* mResourceMappings;
    };
}



#endif    /* RESOURCEMAP_H */

