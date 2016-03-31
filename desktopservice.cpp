/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <string.h>
#include <sys/inotify.h>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <ripc/json.h>
#include <ripc/utils.h>
#include <ripc/notifierresource.h>
#include "handlerTemplate.h"

#include "desktopservice.h"
#include "mimeresourcebuilder.h"
#include "mimeappslistreader.h"
#include "desktopentryreader.h"

#define LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

namespace org_restfulipc {
using namespace std::chrono;
    DesktopService::DesktopService() : Service() 
    {
        iconResource = make_shared<IconResource>();
        setupNotification(); 
        findDirs();
        buildResources();
        watchThread = thread(&DesktopService::watcher, this, setupWatches());
    }

    void DesktopService::setupNotification()
    {
        notifier = make_shared<NotifierResource>();
    }


    void DesktopService::findDirs()
    {
        string home = value("HOME");
        usersConfigDir = value("XDG_CONFIG_HOME", home + "/.config");
        systemConfigDirs = split(value("XDG_CONFIG_DIRS", "/etc/xdg"), ':');
        string xdg_data_home = value("XDG_DATA_HOME", home + "/.local/share");
        usersApplicationDirRoot = xdg_data_home + "/applications";
        for(const string& xdg_data_dir : split(value("XDG_DATA_DIRS", "/usr/share:/usr/local/share"), ':')) { 
            if (xdg_data_dir != xdg_data_home) {
                systemApplicationDirRoots.push_back(xdg_data_dir + "/applications");
            }
        }

    }

    /**
     * Given a directory produces a vector containing, as first, the starting directory, and, after that,
     * all its sub- subsub- etc- directories. All directorypaths end with '/'.
     * Example: Given the directory /usr/share/applications, and assuming /usr/share/applications contains directories
     * xxx and yyy/zzz, the returned vector will lool like:
     * { "/usr/share/applications/", "/usr/share/applications/xxx/", 
     *   "/usr/share/applications/yyy/", "/usr/share/applications/yyy/zzz/"}
     * 
     * 
     * @param directory The directory to start from 
     * @return      
     */
    vector<string> DesktopService::directoryTree(string directory)
    {
        vector<string> directories = { directory + "/" };
        int index = 0; 
        while (index < directories.size()) {
            DIR* dir = opendir(directories[index].data());
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
                else if (dirent->d_name[0] == '.') {
                    // "We skip directories ".", ".." and hidden directories
                    continue;
                }
                else if (dirent->d_type == DT_DIR) {
                    directories.push_back(directories[index] + string(dirent->d_name) + '/');
                }
            }
            closedir(dir);
            index++;
        }
        return directories;
    } 
    
    void DesktopService::watcher(int wd) 
    { 
        char buf[LEN];
        struct inotify_event* ev = (struct inotify_event*)buf;
        struct pollfd pollfd = { wd, POLLIN, 0 };
        for(;;) {
            bool somethingChanged = false;
            if (poll(&pollfd, 1, -1) < 0) throw C_Error();
            for (;;) {
                /* Installations or saves from an editor may produce several inotify events, so 
                 * after receiving an event we continue consuming events until nothing has happened
                 * for a tenth of a second. (TODO: Is this the right value here?)
                 */
                read(wd, buf, LEN);
                std::cout << "Read notification, ev->name: " << ev->name << "\n";
                if (ev->mask & IN_ISDIR) {
                    somethingChanged = true;
                }
                else {
                    if (strcmp("mimeapps.list", ev->name) == 0) {
                        somethingChanged = true; 
                    }
                    else if (strlen(ev->name) > 7 && strcmp(".desktop", ev->name + strlen(ev->name) - 8) == 0) {
                        somethingChanged = true;
                    }
                }

                int pollRes = poll(&pollfd, 1, 100);
                if (pollRes < 0) {
                    throw C_Error();
                }
                else if (pollRes == 0) {
                    break;
                }
            }
           
            if (somethingChanged) {
                std::cout << "Something changed\n";

                buildResources();
            }
            // FIXME
        }
    }
      
    vector<string> DesktopService::desktopFiles(string directory)
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

    int DesktopService::setupWatches()
    {
        int wd = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
        if (wd < 0) throw C_Error();
        addWatch(wd, usersConfigDir); 
        for (const string& configDir: systemConfigDirs) {
            addWatch(wd, configDir);
        }
        for (const string& applicationDir : directoryTree(usersApplicationDirRoot)) {
            addWatch(wd, applicationDir);
        }
        for (const string& systemApplicationDirRoot : systemApplicationDirRoots) {
            for (const string& applicationDir : directoryTree(systemApplicationDirRoot)) {
                addWatch(wd, applicationDir);
            }
        }
        return wd;
    }

    int DesktopService::addWatch(int wd, string dir)
    {
        static int flags = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
        std::cout << "Adding watch for " << dir << "\n";
        int watch = inotify_add_watch(wd, dir.data(), flags) ;
        if (watch < 0) throw C_Error();
        return watch;
    }



    void DesktopService::buildResources()
    {
        std::map<string, LocalizedJsonResource::ptr > oldDesktopResources = desktopResources;
        std::map<string, MimetypeResource::ptr > oldMimetypeResources = mimetypeResources;
        JsonResource::ptr oldRootMimeResource = rootMimeResource;
        JsonResource::ptr oldHandlerResource = handlerResource;
        
        mimetypeResources.clear();
        desktopResources.clear();
        associations.clear();
        defaults.clear();

        resourceMappings.clear(); 
        map("/notify", notifier);

        collectMimetypes();
        
        for (auto it = systemApplicationDirRoots.rbegin(); it != systemApplicationDirRoots.rend(); it++) {
            readDesktopFiles(directoryTree(*it));
            readMimeappsListFile(*it);
        }

        readDesktopFiles(directoryTree(usersApplicationDirRoot));

        for (string configDir : systemConfigDirs) {
            readMimeappsListFile(configDir);
        }

        readMimeappsListFile(usersConfigDir);
        addAssociationsToMimeAndDesktopResources();
        buildFileAndUrlHandlerResource();
        mapResources();

        for (auto pair : desktopResources) {
            if (oldDesktopResources.find(pair.first) != oldDesktopResources.end()) {
                if (oldDesktopResources[pair.first]->json != pair.second->json) {
                    notifier->notifyClients("desktopresource-updated", pair.first.data());
                }
            }
            else {
                notifier->notifyClients("desktopresource-added", pair.first.data());
            }
        }

        for (auto pair : oldDesktopResources) {
            if (desktopResources.find(pair.first) == desktopResources.end()) {
                notifier->notifyClients("desktopresource-removed", pair.first.data());
            }
        }

        for (auto pair : mimetypeResources) {
            if (oldMimetypeResources.find(pair.first) != oldMimetypeResources.end()) {
                if (oldMimetypeResources[pair.first]->json != pair.second->json) {
                    notifier->notifyClients("mimetype-updated", pair.first.data());
                }
            }
            else {
                notifier->notifyClients("mimetype-added", pair.first.data());
            }
        }

        for (auto pair : oldMimetypeResources) {
            if (mimetypeResources.find(pair.first) == mimetypeResources.end()) {
                notifier->notifyClients("mimetype-removed", pair.first.data());
            }
        }

    }

     void DesktopService::collectMimetypes()
    {
        MimeResourceBuilder builder;
        std::cout << "Collected: " << builder.mimetypeResources.size() << " mimetypes\n";
        mimetypeResources.insert(builder.mimetypeResources.begin(), builder.mimetypeResources.end());
        std::cout << "Moved: " << mimetypeResources.size() << "\n";
        rootMimeResource = builder.rootResource;
    }


    void DesktopService::readDesktopFiles(vector<string> applicationsDirs)
    {
        for (const string& applicationsDir : applicationsDirs) {
            for (const string& desktopFilePath : desktopFiles(applicationsDir)) {
                DesktopEntryReader reader(desktopFilePath);
                string entryId = desktopFilePath.data() + applicationsDirs[0].size();
                replace(entryId.begin(), entryId.end(), '/', '-');
                if (reader.json.contains("Hidden") && (bool)reader.json["Hidden"]) {
                    desktopResources.erase(entryId);
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
                    LocalizedJsonResource::ptr desktopResource = make_shared<LocalizedJsonResource>();
                    desktopResource->setJson(std::move(reader.json), std::move(reader.translations));
                    desktopResources[entryId] = desktopResource;
                }
            }
        }
    }

    void DesktopService::readMimeappsListFile(string dir)
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

    void DesktopService::addAssociationsToMimeAndDesktopResources()
    {
        for (auto& p : associations) {
            for (const string& entryId : p.second) {
                if (mimetypeResources.find(p.first) != mimetypeResources.end()) {
                    mimetypeResources[p.first]->json["associatedApplications"].append(entryId);
                }
                if (desktopResources.find(entryId) != desktopResources.end()) {
                    desktopResources[entryId]->json["MimeType"].append(p.first);
                }
            }
        }

        for (const auto& it : defaults) {
            if (it.second.size() > 0 && mimetypeResources.find(it.first) != mimetypeResources.end()) {
                mimetypeResources[it.first]->json["defaultApplication"] = it.second[0];
            }
        }
    }

    void DesktopService::buildFileAndUrlHandlerResource()
    {
        Json handlers;
        handlers << handlerTemplate_json;
        for (const auto& p : desktopResources) {
            if (p.second->json.contains("Exec")) {
                if (strcasestr(p.second->json["Exec"], "%u")) {
                    handlers["fileHandlers"].append(p.first);
                    handlers["urlHandlers"].append(p.first);
                }
                else if (strcasestr(p.second->json["Exec"], "%f")) {
                    handlers["urlHandlers"].append(p.first);
                }
            }
        }
        handlerResource = make_shared<JsonResource>();
        handlerResource->setJson(std::move(handlers));
    }

    void DesktopService::mapResources()
    {
        map("/notify", notifier);
        for (auto& p : desktopResources) {
            string selfUri = string("/desktopentry") + '/' + p.first;
            p.second->json["_links"]["self"]["href"] = selfUri;
            map(selfUri.data(), p.second);
        }

        for (auto& p : mimetypeResources) {
            string selfUri = string("/mimetypes") + '/' + p.first;
            p.second->json["_links"]["self"]["href"] = selfUri;
            p.second->setService(this);
            map(selfUri.data(), p.second);
        }
        map("/mimetypes", rootMimeResource);

        map("/desktopentry/handlers", handlerResource);
        map("/theme-icon/default", iconResource, true);
    }

    bool DesktopService::setDefaultApplication(string mimetype, string desktopEntryId)
    {
        MimeappsList mimeappsList(usersConfigDir + "/mimeapps.list");
        auto& defaultAppsForMime = mimeappsList.defaultApps[mimetype];
        for (auto it = defaultAppsForMime.begin(); it != defaultAppsForMime.end();) {
            if (*it == desktopEntryId) {
                it = defaultAppsForMime.erase(it);
            }
            else {
                it++;
            }
        }
        defaultAppsForMime.insert(defaultAppsForMime.begin(), desktopEntryId);
        mimeappsList.write();
        return true;
    }

    
}

