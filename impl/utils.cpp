#include <string.h>
#include <algorithm>
#include "utils.h"

namespace org_restfulipc
{
    
    vector<string> split(string str, char c)
    {
        std::vector<string> result;
        int k = 0;
        for (int i = 0; i < str.size(); i++) {
            if (str[i] == c) {
                if (i > k) {
                    result.push_back(str.substr(k, i - k));
                }
                k = i + 1;
            }
        }

        if (k < str.size()) {
            result.push_back(str.substr(k));
        }
        return result;
    }

    set<string> splitToSet(string str, char c) {
        vector<string> vals = split(str, c);
        set<string> result;
        result.insert(vals.begin(), vals.end());
        return result; 
    }

    string value(const string& envVarName, const string& fallback)
    {
        const char* val = getenv(envVarName.data());
        return (val && *val) ? val : fallback;
    }

}