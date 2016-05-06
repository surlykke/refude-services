#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include "handlerTemplate.h"
#include "desktopentryresource.h"

namespace org_restfulipc
{

    DesktopEntryResource::DesktopEntryResource(Map<Json>&& desktopJsons) :
        AbstractCachingResource(),
        desktopJsons(move(desktopJsons))
    {
    }

    DesktopEntryResource::~DesktopEntryResource()
    {
    }

    void DesktopEntryResource::setDesktopJsons(Map<Json>&& desktopJsons, NotifierResource::ptr notifier)
    {
        const char* desktopEntryUpdated = "desktopentry-updated";
        const char* desktopEntryAdded = "desktopentry-added";
        const char* desktopEntryRemoved = "desktopentry-removed";
        Map<const char*> notifications;
        {
            unique_lock<recursive_mutex> lock(m);
            clearCache();
            this->desktopJsons.each([&](const char* desktopEntryId, Json& desktopJson){
                if (!desktopJsons.contains(desktopEntryId)) {
                    notifications[desktopEntryId] = desktopEntryRemoved;
                }

            });

            desktopJsons.each([&, this](const char* desktopEntryId, Json& desktopJson) {
                if (! this->desktopJsons.contains(desktopEntryId)) {
                    notifications[desktopEntryId] = desktopEntryAdded;
                }
                else if (this->desktopJsons[desktopEntryId] != desktopJson) {
                    notifications[desktopEntryId] = desktopEntryUpdated;

                }
            });

           this->desktopJsons = move(desktopJsons);
        }
        
        notifications.each([&notifier](const char* desktopEntryId, const char* notificationType) {
            notifier->notifyClients(notificationType, desktopEntryId);
        });

    }

    Buffer DesktopEntryResource::buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers)
    {
        if (*remainingPath == '\0') {
            bool onlyFileHandlers = false;
            bool onlyUrlHandlers = false;

            Json desktopEntryList = JsonConst::EmptyArray;
            request.queryParameterMap.each([&](const char* parameterName, vector<const char*>& values) {
                if (!strcmp("handles", parameterName)) {
                    for (const char* value : values) {
                        if (!strcmp("files", value)) {
                            onlyFileHandlers = true;
                        }
                        else if (!strcmp("urls", value)) {
                            onlyUrlHandlers = true;
                        }
                        else {
                            throw Status::Http422;
                        }
                    }
                }
                else {
                    throw Status::Http422;
                }
            });


            desktopJsons.each([&](const char* desktopEntryId, Json & desktopJson) {
                bool add = false;
                if (onlyFileHandlers) {
                    add = desktopJson.contains("Exec") && strcasestr(desktopJson["Exec"], "%f");
                }    
                else if (onlyUrlHandlers) {
                    add = desktopJson.contains("Exec") && 
                          (strcasestr(desktopJson["Exec"], "%f") || strcasestr(desktopJson["Exec"], "%u"));
                }
                else {
                    add = true;
                }

                if (add) {
                    desktopEntryList.append(desktopEntryId);
                }
            });

            Json result;
            result << handlerTemplate_json;
            result["desktopEntries"] = move(desktopEntryList);
            return JsonWriter(result).buffer;
        } 
        else {
            if (desktopJsons.contains(remainingPath)) {
                return LocalizingJsonWriter(desktopJsons[remainingPath], getAcceptedLocales(request)).buffer;
            }
            else {
                throw Status::Http404;
            }
        }
                     
    }

}