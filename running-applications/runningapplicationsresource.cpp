/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <ripc/json.h>
#include <ripc/jsonwriter.h>

#include "windowinfo.h"
#include "commandsTemplate.h"
#include "runningAppCommandTemplate.h"
#include "runningapplicationsresource.h"

namespace org_restfulipc
{

    bool include(WindowInfo& clientInfo, std::vector<const char*>* searchTerms) {
        if (!clientInfo.isNormal()) {
            return false;
        }

        if (!clientInfo.title) {
            return false;
        }

        if (searchTerms) {
            for (const char* searchTerm : *searchTerms) {
                if (strcasestr(clientInfo.title, searchTerm)) {
                    return true;
                }
            }

            return false;
        }
        else {
            return true;
        }
    }

    RunningApplicationsResource::RunningApplicationsResource() :
        AbstractResource()
    {
    }

    RunningApplicationsResource::~RunningApplicationsResource()
    {
    }

    void RunningApplicationsResource::doGET(int& socket, HttpMessage& request)
    {
         
        Buffer response;
        std::map<std::string, std::string> headers;

        std::vector<const char*>* searchTerms = NULL;

        request.queryParameterMap.each([&searchTerms] (const char* key, std::vector<const char*>& values){
            if (!strcmp("search", key)) {
                searchTerms = &values;
            }
            else {
                throw HttpCode::Http422;
            }
        }); 

        Json commands;
        commands << commandsTemplate_json;
        commands["_links"]["self"]["href"] = mappedTo;
        commands["geometry"] = JsonConst::EmptyObject;

        WindowInfo rootWindow = WindowInfo::rootWindow(); 

        for (Window *client = rootWindow.clients; *client; client++) {
            WindowInfo clientInfo(*client); 
          
            if (! include(clientInfo, searchTerms)) {
                continue;
            }

            Json runningApp;
            runningApp << runningAppCommandTemplate_json;
            runningApp["Name"] = clientInfo.title ? clientInfo.title : "";
            runningApp["Comment"] = "";
            runningApp["geometry"] = JsonConst::EmptyObject;
            runningApp["geometry"]["x"] = clientInfo.x;
            runningApp["geometry"]["y"] = clientInfo.y;
            runningApp["geometry"]["w"] = clientInfo.width;
            runningApp["geometry"]["h"] = clientInfo.height;

            runningApp["_links"]["self"]["href"] = std::string(mappedTo) + "/" + std::to_string(*client);
            runningApp["_links"]["execute"]["href"] = std::string(mappedTo) + "/" + std::to_string(*client);
            runningApp["_links"]["icon"]["href"] = "/icons/icon?name=application-x-executable&size=64";
            runningApp["lastActivated"] = (double) 0;
            commands["commands"].append(std::move(runningApp));
        }


        Buffer content = JsonWriter(commands).buffer;
        buildResponse(response, std::move(content), headers);
        sendFully(socket, response.data(), response.size());
    }

    void RunningApplicationsResource::doPOST(int& socket, HttpMessage& request)
    {
        if (request.remainingPath[0] == '\0') {
            throw HttpCode::Http405;
        }

        errno = 0;
        Window windowToRaise = strtoul(request.remainingPath, NULL, 0);
        if (errno != 0) throw C_Error();

        WindowInfo rootWindow = WindowInfo::rootWindow();

        for (Window *client = rootWindow.clients; *client; client++) {
            if (*client == windowToRaise) {
                std::cout << "raising..\n";
                raiseAndFocus(*client); 
                throw HttpCode::Http204;
            }
        }

        std::cout << "Giving up\n";
        throw HttpCode::Http405;
    }
}