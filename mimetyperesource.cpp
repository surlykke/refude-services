/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/


#include "mimetyperesource.h"
#include <ripc/json.h>
#include <ripc/buffer.h>

#include "desktopservice.h"
namespace org_restfulipc
{

MimetypeResource::MimetypeResource() : LocalizedJsonResource()
{
}

MimetypeResource::~MimetypeResource()
{
}

void MimetypeResource::doPatch(int socket, const HttpMessage& request)
{
    static const char* successfulResponse =
         "HTTP/1.1 204 No Content\r\n"
         "\r\n";

    Json mergeJson;
    mergeJson << request.body;
    if (mergeJson.type() != JsonType::Object) {
        throw Status::Http406;
    }
    else if (mergeJson.size() != 1 ||
             !mergeJson.contains("defaultApplication") ||
             mergeJson["defaultApplication"].type() != JsonType::String) {
        throw Status::Http422; // FIXME Some error message here
    }
    else {
        string thisMimetype = (string)json["type"] + '/' + (string)json["subtype"];
        service->setDefaultApplication(thisMimetype, (string)mergeJson["defaultApplication"]);
        sendFully(socket, successfulResponse, strlen(successfulResponse));        
    }
}

void MimetypeResource::setService(DesktopService* service)
{
    this->service = service;
}


}