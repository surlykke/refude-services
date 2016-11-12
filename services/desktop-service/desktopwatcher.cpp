#include <sys/inotify.h>
#include <poll.h>
#include <limits.h>
#include <unistd.h>
#include <thread>
#include <refude/errorhandling.h>
#include <refude/utils.h>
#include "xdg.h"
#include "desktopwatcher.h"

#define LEN (sizeof(struct inotify_event) + NAME_MAX + 1)

namespace org_restfulipc
{

    DesktopWatcher::DesktopWatcher(Controller& controller, bool emitEventOnStart) : 
        controller(controller),
        emitEventOnStart(emitEventOnStart),
        watchingThread()
    {
    }

    DesktopWatcher::~DesktopWatcher()
    {
    }
    
    void DesktopWatcher::watcher(int wd)
    {
        try {
            if (emitEventOnStart) {
                controller.update();
            }

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
                    controller.update();
                }
            }
        }
        catch (RuntimeError e) {
            std::cerr << "desktopwatcher::watcher caught RuntimeError: " << e.what() << "\n";
            e.printStackTrace();
            std::cerr << "\n";
        }
        catch (...) {
            
        }
    }

    void DesktopWatcher::start()
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
    
        watchingThread = std::thread(&DesktopWatcher::watcher, this, wd);

    }

    int DesktopWatcher::addWatch(int wd, std::string dir)
    {
        static int flags = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
        int watch = inotify_add_watch(wd, dir.data(), flags);
        if (watch < 0) throw C_Error();
        return watch;
    }


}