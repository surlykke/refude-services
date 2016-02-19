#ifndef MIMEAPPSLISTREADER_H
#define MIMEAPPSLISTREADER_H

#include <vector>
#include <map>
#include <string>

/**
 * Based on the algorithm outlined in <FIXME>. 
 * Users of this class should feed it mimeapps.list files in descending order of precedence
 */
namespace org_restfulipc
{
    using namespace std;

    typedef vector<string> AppList;  // Used for lists of applications
    typedef map<string, AppList> AssocMap; // Used to map mimetypes to associated applications

    class MimeappsListReader
    {
    public:
        MimeappsListReader();
        virtual ~MimeappsListReader();
        void read(std::string pathToMimeappsListFile);
        void addAssociation(std::string desktopId, std::string mimetype);
        void resolveDefaults();
        AssocMap associations;
        AssocMap defaults;
        
    private:
        void add(AppList& dest, string applications, const AppList& excludes = AppList());
        
        AssocMap blacklist;
    };

}
#endif /* MIMEAPPSLISTREADER_H */