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

#include "directories.h"
#include "mimeresourcebuilder.h"
#include "desktopentryreader.h"
#include "mimeappslistreader.h"

#include "desktopservice.h"
#include "desktopentryresourcesbuilder.h"
#include "iconresourcebuilder.h"

#define LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

namespace org_restfulipc {
using namespace std::chrono;
    DesktopService::DesktopService() : 
        Service(),
        directories(),
        notifier(),
        watchThread()
    {
        /*notifier = make_shared<NotifierResource>();
        map("/notify", notifier); 
        std::cout << "Building resources from constructor..\n";
        buildResources();
        watchThread = thread(&DesktopService::watcher, this, setupWatches());*/
        IconResourceBuilder iconResourceBuilder;
        iconResourceBuilder.buildResources();
        iconResourceBuilder.mapResources(*this);
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
                 * to allow things to settle down, we continue consuming events until nothing has happened
                 * for a tenth of a second. (TODO: Is this the right value here?)
                 */
                read(wd, buf, LEN);
                std::cout << "Inotify event: " << ev->name << "\n";
                std::cout <<
                    "IN_ACCESS: " << (ev->mask & IN_ACCESS) << "\n" << 
                    "IN_ATTRIB: " << (ev->mask & IN_ATTRIB)  << "\n" << 
                    "IN_CLOSE_WRITE: " << (ev->mask & IN_CLOSE_WRITE)  << "\n" << 
                    "IN_CLOSE_NOWRITE: " << (ev->mask & IN_CLOSE_NOWRITE)  << "\n" << 
                    "IN_CREATE: " << (ev->mask & IN_CREATE)  << "\n" << 
                    "IN_DELETE: " << (ev->mask & IN_DELETE)  << "\n" << 
                    "IN_DELETE_SELF: " << (ev->mask & IN_DELETE_SELF) << "\n" << 
                    "IN_MODIFY: " << (ev->mask & IN_MODIFY)  << "\n" << 
                    "IN_MOVE_SELF: " << (ev->mask & IN_MOVE_SELF) << "\n" << 
                    "IN_MOVED_FROM: " << (ev->mask & IN_MOVED_FROM)  << "\n" << 
                    "IN_MOVED_TO: " << (ev->mask & IN_MOVED_TO)  << "\n" << 
                    "IN_OPEN: " << (ev->mask & IN_OPEN)  << "\n";

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
                std::cout << "Building resources from watcher\n";
                buildResources();
            }
        }
    }
      
    int DesktopService::setupWatches()
    {
        int wd = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
        if (wd < 0) throw C_Error();
        addWatch(wd, directories.usersConfigDir); 
        for (const string& configDir: directories.systemConfigDirs) {
            addWatch(wd, configDir);
        }
        for (const string& applicationDir : directories.directoryTree(directories.usersApplicationDirRoot)) {
            addWatch(wd, applicationDir);
        }
        for (const string& systemApplicationDirRoot : directories.systemApplicationDirRoots) {
            for (const string& applicationDir : directories.directoryTree(systemApplicationDirRoot)) {
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
        std::cout << "Building resources..\n";
        MimeResourceBuilder mimeResourceBuilder;
        mimeResourceBuilder.build();
        DesktopEntryResourceBuilder desktopEntryResourceBuilder;
        desktopEntryResourceBuilder.buildJsons();
        mimeResourceBuilder.addAssociationsAndDefaults(desktopEntryResourceBuilder.associations, 
                                                       desktopEntryResourceBuilder.defaults);        
        mimeResourceBuilder.mapResources(*this, notifier);
        desktopEntryResourceBuilder.mapResources(*this, notifier);
    }

    
}
