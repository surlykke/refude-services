#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <string>

namespace org_restfulipc
{
    using namespace std;

    typedef vector<string> AppList;  // Used for lists of applications
    typedef map<string, AppList> MimeAppMap; // Used to map mimetypes to associated applications

}


#endif /* TYPEDEFS_H */

