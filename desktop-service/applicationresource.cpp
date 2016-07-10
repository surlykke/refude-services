#include "runapplication.h"
#include "applicationresource.h"

namespace org_restfulipc
{
    ApplicationsResource::ApplicationsResource() :
        LocalizedJsonResource()
    {
    }

    ApplicationsResource::~ApplicationsResource()
    {
    }

    void ApplicationsResource::doPOST(int& socket, HttpMessage& request)
    {

        if (! getJson().contains("Exec")) {
            throw RuntimeError("No 'Exec' field\n");
        }

        runApplication((const char*) getJson()["Exec"]);
        throw HttpCode::Http204;
 
    }

}