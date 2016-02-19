#ifndef MIMEAPPSLISTREADER_H
#define MIMEAPPSLISTREADER_H

#include <vector>
#include <map>
#include <string>
#include "utils.h"
/**
 * Based on the algorithm outlined in <FIXME>. 
 * Users of this class should feed it mimeapps.list files in descending order of precedence
 */
namespace org_restfulipc
{
    using namespace std;

    class MimeappsListReader
    {
    public:
        MimeappsListReader();
        virtual ~MimeappsListReader();
        void read(std::string pathToMimeappsListFile);
        void addAssociation(std::string desktopId, std::string mimetype);
        void resolveDefaults();
        MimeAppMap associations;
        MimeAppMap defaults;
        
    private:
        void add(AppList& dest, string applications, const AppList& excludes = AppList());
        
        MimeAppMap blacklist;
    };
}
#endif /* MIMEAPPSLISTREADER_H */