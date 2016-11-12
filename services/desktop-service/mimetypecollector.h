#ifndef MIMETYPECOLLECTOR_H
#define MIMETYPECOLLECTOR_H
#include <set>
#include <refude/map.h>
#include <refude/json.h>

namespace refude 
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

