#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include <unistd.h>
#include "handlerTemplate.h"
#include "commandsTemplate.h"
#include "runapplication.h"
#include "desktopentryresource.h"

namespace org_restfulipc
{

    DesktopEntryResource::DesktopEntryResource(Map<Json>&& desktopJsons) :
        AbstractCachingResource(),
        desktopJsons(std::move(desktopJsons))
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
            std::unique_lock<std::recursive_mutex> lock(m);
            clearCache();
            this->desktopJsons.each([&](const char* desktopEntryId, Json & desktopJson)
            {
                if (!desktopJsons.contains(desktopEntryId)) {
                    notifications[desktopEntryId] = desktopEntryRemoved;
                }

            });

            desktopJsons.each([&, this](const char* desktopEntryId, Json & desktopJson)
            {
                if (!this->desktopJsons.contains(desktopEntryId)) {
                    notifications[desktopEntryId] = desktopEntryAdded;
                }
                else if (this->desktopJsons[desktopEntryId] != desktopJson) {
                    notifications[desktopEntryId] = desktopEntryUpdated;

                }
            });

            this->desktopJsons = std::move(desktopJsons);
        }

        notifications.each([&notifier](const char* desktopEntryId, const char* notificationType)
        {
            notifier->notifyClients(notificationType, desktopEntryId);
        });

    }

    void DesktopEntryResource::doPOST(int& socket, HttpMessage& request, const char* remainingPath)
    {
        std::cout << "doPost: '" << remainingPath << "'\n";
        if (!strncmp(remainingPath, "commands/", strlen("commands/"))) {
            const char* desktopEntryId = remainingPath + strlen("commands/") ;
            if (desktopJsons.contains(desktopEntryId)) {
                Json& desktopJson = desktopJsons[desktopEntryId];

                if (!desktopJson.contains("Exec")) {
                    throw RuntimeError("No 'Exec' field in %s\n", desktopEntryId);
                }

                runApplication((const char*)desktopJson["Exec"]);
                
                throw HttpCode::Http200;
            }
            else {
                throw HttpCode::Http404;
            }
        }
        throw HttpCode::Http406;
    }

    Buffer DesktopEntryResource::buildContent(HttpMessage& request, 
                                              const char* remainingPath, 
                                              std::map<std::string, std::string>& headers)
    {
        std::cout << "DesktopEntryResource, path: " << request.path << "\n"
                  << "- remaining path: " << remainingPath << "\n";
        if (*remainingPath == '\0') {
            bool onlyFileHandlers = false;
            bool onlyUrlHandlers = false;
            std::vector<const char*>* searchTerms = NULL;
            std::vector<std::string> locales = getAcceptedLocales(request);
            Json desktopEntryList = JsonConst::EmptyArray;
            request.queryParameterMap.each([&](const char* parameterName, 
                                               std::vector<const char*>& values)
            {
                if (!strcmp("handles", parameterName)) {
                    for (const char* value : values) {
                        if (!strcmp("files", value)) {
                            onlyFileHandlers = true;
                        }
                        else if (!strcmp("urls", value)) {
                            onlyUrlHandlers = true;
                        }
                        else {
                            throw HttpCode::Http422;
                        }
                    }
                }
                else if (!strcmp("search", parameterName)) {
                    searchTerms = &values;
                }
                else {
                    throw HttpCode::Http422;
                }
            });


            desktopJsons.each([&](const char* desktopEntryId, Json & desktopJson)
            {
                bool add;
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

                if (searchTerms) {
                              add = matchDesktopEntry(desktopJson, searchTerms, locales);
                }

                if (add) {
                              desktopEntryList.append(desktopEntryId);
                }
            });

            Json result;
            result << handlerTemplate_json;
            result["desktopEntries"] = std::move(desktopEntryList);
            return JsonWriter(result).buffer;
        }
        else if (!strcmp(remainingPath, "commands")) {
            Json content;
            content << commandsTemplate_json;
            std::vector<const char*>* searchTerms = NULL;
            std::vector<std::string> locales = getAcceptedLocales(request);
            request.queryParameterMap.each([&searchTerms](const char* parameterName, 
                                                          std::vector<const char*>& values)
            {
                if (!strcmp("search", parameterName)) {
                    searchTerms = &values;
                }
                else {
                    throw HttpCode::Http422;
                }
            });

            desktopJsons.each([&](const char* desktopEntryId, Json & desktopJson)
            {
                Json command = JsonConst::EmptyObject;
                if (matchCommand(desktopJson, searchTerms, locales)) {
                              command["Name"] = desktopJson["Name"].copy();
                              command["Comment"] = desktopJson["Comment"].copy();
                              command["Icon"] = (const char*) desktopJson["Icon"];
                              command["Exec"] = (const char*) desktopJson["Exec"];
                              command["TypeId"] = "desktopentries";
                              command["Id"] = desktopEntryId;
                              content["commands"].append(std::move(command));
                }
            });

            return LocalizingJsonWriter(content, locales).buffer;
        }
        else {
            std::cout << "Looking at " << remainingPath << "\n";
            if (desktopJsons.contains(remainingPath)) {
                return LocalizingJsonWriter(desktopJsons[remainingPath], getAcceptedLocales(request)).buffer;
            }
            else {
                throw HttpCode::Http404;
            }
        }

    }

    bool DesktopEntryResource::matchCommand(Json& desktopJson, 
                                            std::vector<const char*>* searchTerms, 
                                            const std::vector<std::string>& locales)
    {
        if (!searchTerms) {
            return true;
        }

        Json& nameObj = desktopJson["Name"];

        for (const char* searchTerm : *searchTerms) {
            for (std::string locale : locales) {
                if (nameObj.contains(locale) && strcasestr(nameObj[locale], searchTerm)) {
                    return true;
                }
            }

            if (strcasestr(nameObj["_ripc:localized"], searchTerm)) {
                return true;
            }
        }

        return false;
    }

    bool DesktopEntryResource::matchDesktopEntry(Json& desktopJson, 
                                                 std::vector<const char*>* searchTerms, 
                                                 const std::vector<std::string>& locales)
    {
        if (!searchTerms) {
            return true;
        }

        Json& nameObj = desktopJson["Name"];

        for (const char* searchTerm : *searchTerms) {
            for (std::string locale : locales) {
                if (nameObj.contains(locale) && strcasestr(nameObj[locale], searchTerm)) {
                    return true;
                }
            }

            if (nameObj.contains("") && strcasestr(nameObj[""], searchTerm)) {
                return true;
            }
        }

        return false;
    }
}