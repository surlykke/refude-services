#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <set>
#include <string>

namespace org_restfulipc 
{
    using namespace std;

    vector<string> split(string str, char c);
    set<string> splitToSet(string str, char c);
    string value(const string& envVarName, const string& fallback = "");
}
#endif /* UTILS_H */

