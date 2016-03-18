#ifndef MIMEAPPSLISTREADER_H
#define MIMEAPPSLISTREADER_H
#include <ostream>
#include "typedefs.h"
/**
 * Based on the algorithm outlined in <FIXME>. 
 * Users of this class should feed it mimeapps.list files in descending order of precedence
 */
namespace org_restfulipc
{
    using namespace std;

    struct MimeappsList 
    {
        MimeappsList(std::string path);
        ~MimeappsList() {}
        void write(); 
        AppLists defaultApps;
        AppSets addedAssociations;
        AppSets removedAssociations;
        
    private:
        string filePath;

    };
}
#endif /* MIMEAPPSLISTREADER_H */