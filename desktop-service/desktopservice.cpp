/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <sys/inotify.h>
#include <poll.h>
#include <limits.h>
#include <unistd.h>
#include <set>
#include "service.h"
#include "resources.h"
#include "jsonresourcecollection.h"
#include "applicationcollector.h"
#include "mimetypecollector.h"
#include "utils.h"
#include "xdg.h"

#include "desktopservice.h"

namespace refude
{
    namespace DesktopService
    {
        #define LEN (sizeof(struct inotify_event) + NAME_MAX + 1)


        JsonResourceCollection resourceCollection;

        Json buildActions(Json& application)
        {
            Json actions = JsonConst::EmptyObject;
            Json action = JsonConst::EmptyObject;
            action["_ripc:localized:name"] = application["_ripc:localized:Name"].copy();
            action["_ripc:localized:comment"] = application["_ripc:localized:Comment"].copy();
            action["icon"] = application["Icon"].copy();
            action["exec"] = application["Exec"].copy();
            actions["_default"] = std::move(action);
            if (application.contains("Actions")) {
                application["Actions"].eachEntry(
                    [&application, &actions](const std::string& key, Json& value) {
                        Json action = JsonConst::EmptyObject;
                        action["_ripc:localized:name"] = value["_ripc:localized:Name"].copy();
                        action["_ripc:localized:comment"] = application["_ripc:localized:Name"].copy();
                        action["icon"] = application["Icon"].copy();
                        action["exec"] = value["Exec"].copy();
                        actions[key] = std::move(action);
                    }
                );
                application.erase("Actions");
            }
            application["_actions"] = std::move(actions);
        }

        void collectApplicationsAndMimetypes()
        {
            std::cout << "collectApplicationsAndMimetypes\n";
            ApplicationCollector applicationCollector;
            applicationCollector.collect();

            MimetypeCollector mimetypeCollector;
            mimetypeCollector.collect();

            mimetypeCollector.addAssociations(applicationCollector.collectedApplications);
            mimetypeCollector.addDefaultApplications(applicationCollector.defaultApplications);

            Json applicationPaths = JsonConst::EmptyArray;
            Json mimetypePaths = JsonConst::EmptyArray;
            Map<JsonResource::ptr> newResources;

            for (auto& entry : applicationCollector.collectedApplications) {
                buildActions(entry.value);
                std::string path = std::string("/application/") + entry.key;
                newResources[path] =  std::make_unique<ApplicationResource>(std::move(entry.value));
                applicationPaths.append(path.substr(1));
            };

            newResources["/applications"] = std::make_unique<JsonResource>(std::move(applicationPaths));

            for (auto& entry: mimetypeCollector.collectedMimetypes) {
                std::string path = std::string("/mimetype/") + entry.key;
                newResources[path] = std::make_unique<MimetypeResource>(std::move(entry.value));
                mimetypePaths.append(path.substr(1));
            };

            newResources["/mimetypes"] = std::make_unique<JsonResource>(std::move(mimetypePaths));

            resourceCollection.updateCollection(std::move(newResources));
        }

        int addWatch(int wd, std::string dir)
        {
            static int flags = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
            int watch = inotify_add_watch(wd, dir.data(), flags);
            if (watch < 0) throw C_Error();
            return watch;
        }

        void watchDesktopFiles()
        {
            std::cout << "watching\n";
            int wd = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
            if (wd < 0) throw C_Error();
            addWatch(wd, xdg::config_home());
            for (const std::string& configDir : xdg::config_dirs()) {
                addWatch(wd, configDir);
            }
            std::string usersApplicationsDir = xdg::data_home() + "/applications";
            for (const std::string& applicationDir : directoryTree(usersApplicationsDir)) {
                addWatch(wd, applicationDir);
            }
            std::vector<std::string> sysApplicationsDirs = append(xdg::data_dirs(), "/applications");
            for (const std::string& systemApplicationDirRoot : sysApplicationsDirs) {
                std::vector<std::string> tree = directoryTree(systemApplicationDirRoot);
                for (const std::string& applicationDir : directoryTree(systemApplicationDirRoot)) {
                    addWatch(wd, applicationDir);
                }
            }

            collectApplicationsAndMimetypes();

            char buf[LEN];
            struct inotify_event* ev = (struct inotify_event*) buf;
            struct pollfd pollfd = {wd, POLLIN, 0};
            for (;;) {
                bool somethingChanged = false;
                if (poll(&pollfd, 1, -1) < 0) throw C_Error();
                for (;;) {
                    /* Installations or saves from an editor may produce several inotify events, so
                     * to allow things to settle down, we continue consuming events until nothing has happened
                     * for a tenth of a second. (TODO: Is this the right value here?)
                     */
                    read(wd, buf, LEN);

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
                    collectApplicationsAndMimetypes();
                }
            }
        }

        // Public
        void run()
        {
            service::run();
            service::listen(xdg::runtime_dir() + "/org.refude.desktop-service");
            watchDesktopFiles();
        }
    }
}
