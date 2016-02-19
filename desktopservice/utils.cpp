#include <string.h>
#include "utils.h"

namespace org_restfulipc
{
    vector<string> split(string str, char c)
    {
        std::vector<std::string> result;
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

    string value(const char* envVarName)
    {
        return getenv(envVarName) && strlen(getenv(envVarName)) > 0 ? getenv(envVarName) : "";
    }

}