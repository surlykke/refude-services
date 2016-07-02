#include <fstream>
#include <unistd.h>
#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include <ripc/localizingjsonwriter.h>
#include <ripc/buffer.h>
#include "handlerTemplate.h"
#include "commandTemplate.h"
#include "commandsTemplate.h"
#include "runapplication.h"
#include "desktopentryresource.h"
#include "xdg.h"

namespace org_restfulipc
{
    static const std::string lastUsedFilePath =
        xdg::config_home() + "/RefudeService/desktopEntryResourceLastUsed.json";

    DesktopEntryResource::DesktopEntryResource(Map<Json>&& desktopJsons) :
        AbstractCachingResource(),
        desktopJsons(std::move(desktopJsons))
    {
        try {
            Buffer buf = Buffer::fromFile(lastUsedFilePath.data());
            commandLastUsed << buf.data();
        }
        catch (...) {
            commandLastUsed = JsonConst::EmptyObject;
        }
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
            std::lock_guard<std::recursive_mutex> lock(m);
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

    void DesktopEntryResource::doPOST(int& socket, HttpMessage& request)
    {
        if (request.remainingPath == NULL || !desktopJsons.contains(request.remainingPath)) {
            throw HttpCode::Http404;
        }

        Json& desktopJson = desktopJsons[request.remainingPath];

        if (!desktopJson.contains("Exec")) {
            throw RuntimeError("No 'Exec' field in %s\n", request.remainingPath);
        }

        runApplication((const char*) desktopJson["Exec"]);
        commandLastUsed[(const char*) request.remainingPath] = (double) time(NULL);
        JsonWriter(commandLastUsed).buffer.toFile(lastUsedFilePath.data());

        throw HttpCode::Http204;
    }

    Buffer DesktopEntryResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        if (request.remainingPath[0] == '\0') {
            return handleDesktopEntrySearch(request);
        }
        else if (!strcmp(request.remainingPath, "commands")) {
            return handleCommandSearch(request);
        }
        else {
            if (desktopJsons.contains(request.remainingPath)) {
                Json& desktopJson = desktopJsons[request.remainingPath];
                desktopJson["_links"]["self"]["href"] = std::string("/desktopentries/") + request.remainingPath;
                return LocalizingJsonWriter(desktopJsons[request.remainingPath],
                                            getAcceptedLocales(request)).buffer;
            }
            else {
                throw HttpCode::Http404;
            }
        }
    }

    Buffer DesktopEntryResource::handleDesktopEntrySearch(HttpMessage& request)
    {
        bool onlyFileHandlers = false;
        bool onlyUrlHandlers = false;
        std::vector<const char*>* searchTerms = NULL;
        std::vector<std::string> locales = getAcceptedLocales(request);
        Json desktopEntryList = JsonConst::EmptyArray;
        request.queryParameterMap.each([&](const char* parameterName, std::vector<const char*>& values)
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

        std::string selfRef = "/desktopentries";
        char separator = '?';
        request.queryParameterMap.each([&selfRef, &separator](const char* key, std::vector<const char*>& values)
        {
            for (const char* value : values) {
                selfRef.append(1, separator).append(key).append(1, '=').append(value);
                separator = '&';
            }
        });

        result["_links"]["self"]["href"] = selfRef;

        result["_links"]["application"]["href"] = "/desktopentries/{desktopEntryId}";

        result["desktopEntries"] = std::move(desktopEntryList);
        return JsonWriter(result).buffer;

    }

    Buffer DesktopEntryResource::handleCommandSearch(HttpMessage& request)
    {
        Json content;
        std::string selfRef = "/desktopentries/commands";
        content << commandsTemplate_json;
        std::vector<const char*>* searchTerms = NULL;
        std::vector<std::string> locales = getAcceptedLocales(request);
        request.queryParameterMap.each([&searchTerms, &selfRef](const char* parameterName,
                                       std::vector<const char*>& values)
        {
            if (!strcmp("search", parameterName)) {
                searchTerms = &values;
                char separator = '?';
                for (const char* value : values) {
                    selfRef.append(1, separator).append("search=").append(value);
                }
            }
            else {
                throw HttpCode::Http422;
            }
        });

        content["_links"]["self"]["href"] = selfRef;

        desktopJsons.each([&, this](const char* desktopEntryId, Json & desktopJson)
        {
            if (matchDesktopEntry(desktopJson, searchTerms, locales)) {
                Json command;
                command << commandTemplate_json;
                command["_ripc:localized:Name"] = desktopJson["_ripc:localized:Name"].copy();
                command["_ripc:localized:Comment"] = desktopJson["_ripc:localized:Comment"].copy();
                command["Icon"] = (const char*) desktopJson["Icon"];
                command["Exec"] = (const char*) desktopJson["Exec"];
                command["_links"]["self"]["href"] = std::string("/desktopentries/") + desktopEntryId;
                command["_links"]["execute"]["href"] = std::string("/desktopentries/") + desktopEntryId;
                if (commandLastUsed.contains(desktopEntryId)) {
                    command["lastActivated"] = (double) commandLastUsed[desktopEntryId];
                }
                else {
                    command["lastActivated"] = (double) 0;
                }
                content["commands"].append(std::move(command));
            }
        });

        return LocalizingJsonWriter(content, locales).buffer;

    }

    bool DesktopEntryResource::matchDesktopEntry(Json& desktopJson,
                                                 std::vector<const char*>* searchTerms,
                                                 const std::vector<std::string>& locales)
    {

        if (desktopJson.contains("NoDisplay")) {
            if ((bool)desktopJson["NoDisplay"]) {
                return false;
            }
        }

        if (!searchTerms) {
            return true;
        }

        Json& nameObj = desktopJson["_ripc:localized:Name"];

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