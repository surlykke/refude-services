#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include "refudeMainResourceTemplate.h"
#include "refudemainresource.h"

namespace org_restfulipc
{

    RefudeMainResource::RefudeMainResource() : AbstractCachingResource()
    {
    }

    RefudeMainResource::~RefudeMainResource()
    {
    }

    Buffer RefudeMainResource::buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers)
    {
        Json refudeMainJson;
        refudeMainJson << refudeMainResourceTemplate_json;
        return JsonWriter(refudeMainJson).buffer;
    }

}