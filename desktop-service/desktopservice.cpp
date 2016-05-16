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

#include "xdg.h"
#include "mimeresourcebuilder.h"
#include "desktopentryresourcesbuilder.h"
#include "desktopentryreader.h"
#include "mimeappslistreader.h"
#include "typedefs.h"
#include "xdg.h"


#include "desktopservice.h"

#define LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

namespace org_restfulipc
{

    DesktopResources::DesktopResources() :
        notifier(),
        watchThread()
    {
    }

    void DesktopResources::setup(Service& service)
    {
        DesktopEntryResourceBuilder desktopEntryResourceBuilder;
        desktopEntryResourceBuilder.build();
        MimeResourceBuilder mimeResourceBuilder;
        mimeResourceBuilder.build();
        mimeResourceBuilder.addAssociationsAndDefaults(desktopEntryResourceBuilder.desktopJsons,
                                                       desktopEntryResourceBuilder.defaultApplications);
        notifier = std::make_shared<NotifierResource>();

        desktopEntryResource =
            std::make_shared<DesktopEntryResource>(std::move(desktopEntryResourceBuilder.desktopJsons));
         
        service.map("/desktopentries", desktopEntryResource, true);

        mimetypeResource =
            std::make_shared<MimetypeResource>(std::move(mimeResourceBuilder.mimetypeJsons));
        service.map("/mimetypes", mimetypeResource, true);

        service.map("/notify", notifier);
        watchThread = std::thread(&DesktopResources::watcher, this, setupWatches());

    }

    void DesktopResources::watcher(int wd)
    {
        try {
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
                    buildResources();
                }
            }
        }
        catch (RuntimeError e) {
            std::cerr << "DesktopService::watcher caught RuntimeError: " << e.what() << "\n";
            e.printStackTrace();
            std::cerr << "\n";
        }
        catch (...) {

        }
    }

    int DesktopResources::setupWatches()
    {
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
        return wd;
    }

    int DesktopResources::addWatch(int wd, std::string dir)
    {
        static int flags = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
        int watch = inotify_add_watch(wd, dir.data(), flags);
        if (watch < 0) throw C_Error();
        return watch;
    }

    void DesktopResources::buildResources()
    {
        MimeResourceBuilder mimeResourceBuilder;
        mimeResourceBuilder.build();
        DesktopEntryResourceBuilder desktopEntryResourceBuilder;
        desktopEntryResourceBuilder.build();
        mimeResourceBuilder.addAssociationsAndDefaults(desktopEntryResourceBuilder.desktopJsons,
                                                       desktopEntryResourceBuilder.defaultApplications);
        mimetypeResource->setMimetypeJsons(std::move(mimeResourceBuilder.mimetypeJsons), notifier);
        desktopEntryResource->setDesktopJsons(std::move(desktopEntryResourceBuilder.desktopJsons), notifier);
    }


}
