#ifndef JSONBUILDER_H
#define JSONBUILDER_H

#include <json.h>

using namespace org_restfulipc;
using namespace std;

class ResourceBuilder
{
public:
    ResourceBuilder(const char* mimedir);
private:

    void read(const char* xmlFilePath);
    char* dup(const char* str);

    int stringRepoUsed;
    int stringRepoCapacity;
    char* stringRepo;
};


#endif // JSONBUILDER_H
