/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   displayresource.cpp
 * Author: Christian Surlykke
 * 
 * Created on 28. maj 2016, 23:20
 */

#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include "windowinfo.h"
#include "displayResourceTemplate.h"
#include "displayresource.h"

namespace org_restfulipc
{

    DisplayResource::DisplayResource()
    {
    }

    DisplayResource::~DisplayResource()
    {
    }

    Buffer DisplayResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        Json displayJson;
        displayJson << displayResourceTemplate_json;
        displayJson["_links"]["self"]["href"] = mappedTo;
        Json& geometry = displayJson["geometry"];
        WindowInfo rootWindowInfo = WindowInfo::rootWindow();
        geometry["x"] = rootWindowInfo.x;
        geometry["y"] = rootWindowInfo.y;
        geometry["h"] = rootWindowInfo.height;
        geometry["w"] = rootWindowInfo.width;

        return JsonWriter(displayJson).buffer;
    }

}