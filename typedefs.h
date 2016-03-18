#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <set>
#include <list>
#include <string>

namespace org_restfulipc
{
    using namespace std;

    typedef map<string, vector<string> > AppLists;
    typedef map<string, set<string> > AppSets;

}


#endif /* TYPEDEFS_H */

