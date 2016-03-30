/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mymetyperesource.cpp
 * Author: Christian Surlykke
 * 
 * Created on 17. marts 2016, 20:24
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