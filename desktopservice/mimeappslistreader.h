#ifndef MIMEAPPSLISTREADER_H
#define MIMEAPPSLISTREADER_H

#include <vector>
#include <map>
#include <string>
#include "typedefs.h" 
/**
 * Based on the algorithm outlined in <FIXME>. 
 * Users of this class should feed it mimeapps.list files in descending order of precedence
 */
namespace org_restfulipc
{
    using namespace std;

    class MimeappsList 
    {
    public:
        MimeappsList(std::string path);
        ~MimeappsList() {}
        MimeAppMap defaultApps;
        MimeAppMap addedAssociations;
        MimeAppMap removedAssociations;
    };

    class MimeappsListCollector
    {
    public:
        MimeappsListCollector();
        virtual ~MimeappsListCollector();
        void collect(const MimeappsList& mimeappsList);
        void addAssociation(std::string mimetype, std::string desktopId);
        void blacklistAssociation(std::string mimetype, std::string desktopId);

        MimeAppMap defaults;
        MimeAppMap associations;
        MimeAppMap blacklist;
       
    private:
        bool contains(const AppList& applist, string desktopId);
    };
}
#endif /* MIMEAPPSLISTREADER_H */