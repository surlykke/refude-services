#ifndef MIMETYPECOLLECTOR_H
#define MIMETYPECOLLECTOR_H
#include <set>
#include <ripc/map.h>
#include <ripc/json.h>

namespace org_restfulipc 
{
    struct MimetypeCollector
    {
        MimetypeCollector();
        virtual ~MimetypeCollector();
        void collect(); 
        void addAssociations(Json& applicationArray);
        void addDefaultApplications(Map<std::vector<std::string>>& defaultApplications);
        
        Json jsonArray;
    };
}

#endif /* MIMETYPECOLLECTOR_H */

