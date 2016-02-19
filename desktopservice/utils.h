#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <string>

namespace org_restfulipc 
{
    using namespace std;

    vector<string> split(string str, char c);
    string value(const char* envVarName);
}
#endif /* UTILS_H */

