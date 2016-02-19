#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <map>
#include <string>

namespace org_restfulipc 
{
    using namespace std;

    typedef vector<string> AppList;  // Used for lists of applications
    typedef map<string, AppList> MimeAppMap; // Used to map mimetypes to associated applications

    vector<string> split(string str, char c);
    string value(const char* envVarName);
}
#endif /* UTILS_H */

