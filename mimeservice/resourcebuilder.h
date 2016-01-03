#ifndef JSONBUILDER_H
#define JSONBUILDER_H

#include <json.h>
#include <buffer.h>
#include <list.h>
#include <service.h>

using namespace std;
namespace org_restfulipc
{
    class JsonResource;

    class ResourceBuilder : public Service
    {
    public:
        ResourceBuilder(const char* mimedir);
    private:

        void read(const char* xmlFilePath);
        JsonResource* buildMimeTypesResource();
        JsonResource* buildToplevelResource(const char* selfUriTmp);
        JsonResource* buildSubTypeResource(const char* typeString);

        List<char*>* strings;
    };

}

#endif // JSONBUILDER_H
