/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <errno.h>
#include <dirent.h>
#include <ripc/utils.h>
#include <ripc/notifierresource.h>
#include <ripc/jsonwriter.h>

#include "desktopentryreader.h"
#include "mimeappslistreader.h"
#include "handlerTemplate.h"

#include "desktopentryresourcesbuilder.h"

namespace org_restfulipc
{
    DesktopEntryResourceBuilder::DesktopEntryResourceBuilder() : 
        associations(),
        defaults(),
        desktopJsons(),
        translations(),
        handlers(), 
        directories()
    {
    }

    DesktopEntryResourceBuilder::~DesktopEntryResourceBuilder()
    {
    }

    void DesktopEntryResourceBuilder::buildJsons()
    {

        for (auto it = directories.systemApplicationDirRoots.rbegin(); it != directories.systemApplicationDirRoots.rend(); it++) {
            readDesktopFiles(directories.directoryTree(*it));
            readMimeappsListFile(*it);
        }

        readDesktopFiles(directories.directoryTree(directories.usersApplicationDirRoot));

        for (string configDir : directories.systemConfigDirs) {
            readMimeappsListFile(configDir);
        }

        readMimeappsListFile(directories.usersConfigDir);

        for (auto& p : associations) {
            for (const string& entryId : p.second) {
                string url = string("/desktopentry/") + entryId;
                if (desktopJsons.find(url.data()) >= 0) {
                    desktopJsons[url]["MimeType"].append(p.first);
                }
            }
        }

        buildFileAndUrlHandlerResource();
    }

    void DesktopEntryResourceBuilder::mapResources(Service& service, NotifierResource::ptr notifier)
    {
        vector<const char*> mappedUrls = service.resourceMappings.keys("/desktopentry/");
        vector<const char*> removedUrls;
        for (const char* url : mappedUrls) {
            if (desktopJsons.find(url) < 0) {
                removedUrls.push_back(url);
            }
        }
        for (const char* url : removedUrls) {
            const char* entryId = url + strlen("/desktopentry/");
            notifier->notifyClients("desktopentry-removed", entryId);
            service.unMap(url);
        }
        
        for (const char* url : desktopJsons.keys()) {
            const char* entryId = url + strlen("/desktopentry/");
            LocalizedJsonResource::ptr resource = dynamic_pointer_cast<LocalizedJsonResource>(service.mapping(url));
            if (resource) {
                if (resource->json != desktopJsons[url] || resource->translations != translations[url]) {
                    resource->setJson(move(desktopJsons[url]), move(translations[url]));
                    notifier->notifyClients("desktopentry-updated", entryId);
                }
            }
            else {
                resource = make_shared<LocalizedJsonResource>();
                resource->setJson(move(desktopJsons[url]), move(translations[url]));
                notifier->notifyClients("desktopentry-created", entryId);
                service.map(url, resource);
            }
        }

        JsonResource::ptr handlerResource = dynamic_pointer_cast<JsonResource>(service.mapping("/handlers"));
        if (handlerResource) {
            if (! handlerResource->equal(handlers)) {
                handlerResource->setJson(move(handlers));
                notifier->notifyClients("handlers-updated", "");
            }
        }
        else {
            handlerResource = make_shared<JsonResource>();
            handlerResource->setJson(move(handlers));
            service.map("/handlers", handlerResource);
            notifier->notifyClients("handlers-created", "");
        }
    }

    vector<string> DesktopEntryResourceBuilder::desktopFiles(string directory)
    {
        vector<string> files;
        DIR* dir = opendir(directory.data());
        if (dir == NULL) throw C_Error();
        
        for (;;) {
            errno = 0;
            struct dirent* dirent = readdir(dir);
            if (errno && !dirent) { 
                throw C_Error();
            }
            else if (!dirent) {
                break;
            } 
            else if ( (dirent->d_type == DT_REG || dirent->d_type == DT_LNK) && 
                        (strlen(dirent->d_name) > 8 && 
                        !strcmp(".desktop", dirent->d_name + strlen(dirent->d_name) - 8))) {
                files.push_back(directory + dirent->d_name);        
            }
        }
       
        closedir(dir);
        return files;
    }
    
    void DesktopEntryResourceBuilder::readDesktopFiles(vector<string> applicationsDirs)
    {
        for (const string& applicationsDir : applicationsDirs) {
            for (const string& desktopFilePath : desktopFiles(applicationsDir)) {
                DesktopEntryReader reader(desktopFilePath);
                string entryId = replaceAll(string(desktopFilePath.data() + applicationsDirs[0].size()), '/', '-');
                string url = "/desktopentry/" + entryId;
                if (reader.json.contains("Hidden") && (bool)reader.json["Hidden"]) {
                    desktopJsons.erase(url.data());
                    for (auto& it : associations) {
                        it.second.erase(entryId);
                    }
                }
                else {
                    if (reader.json.contains("MimeType")) {
                        while (reader.json["MimeType"].size() > 0) {
                            associations[(const char*)reader.json["MimeType"].take(0)].insert(entryId);
                        }
                        // We will restore this array later - see below
                    }
                    desktopJsons[url] = std::move(reader.json);
                    translations[url] = std::move(reader.translations);
                }
            }
        }
    }

    void DesktopEntryResourceBuilder::readMimeappsListFile(string dir)
    {
        MimeappsList mimeappsList(dir + "/mimeapps.list");
        for (const auto& p : mimeappsList.removedAssociations) {
            for (string entryId : p.second) {
                associations[p.first].erase(entryId);
            }
        }

        for (const auto& p : mimeappsList.removedAssociations) {
            for (string entryId : p.second) {
                associations[p.first].erase(entryId);
            }
        }

        for (const auto& p : mimeappsList.defaultApps) {
            const string& mimeType = p.first;
            for (const string& entryId : p.second) {
                defaults[mimeType].push_back(entryId);
            }
        }
    }

    void DesktopEntryResourceBuilder::buildFileAndUrlHandlerResource()
    {
        handlers << handlerTemplate_json;
        for (const char* url : desktopJsons.keys()) {
            const char* entryId = url + strlen("/desktopentry/");
            Json& desktopentryJson = desktopJsons[url];
            if (desktopentryJson.contains("Exec")) {
                if (strcasestr(desktopentryJson["Exec"], "%u")) {
                    handlers["fileHandlers"].append(entryId);
                    handlers["urlHandlers"].append(entryId);
                }
                else if (strcasestr(desktopentryJson["Exec"], "%f")) {
                    handlers["urlHandlers"].append(entryId);
                }
            }
        }
    }


}
