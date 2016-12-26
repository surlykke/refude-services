#include <tuple>
#include "app.h"

namespace refude
{

    Task::~Task()
    {
    }

    void App::run()
    {
        for (int i = 0; i < 5; i++) {
            inst().backgroundThreads.push_back(std::thread(&App::backgroundWorker, &inst()));
        }

        for(;;) {
            (inst().tasksForMainThread.dequeue())->execute();
        }
    }

    App& App::inst()
    {
        static App mainApp;
        return mainApp;
    }

    App::App() : tasksForMainThread(), tasksForBackgroundThreads(), backgroundThreads()
    {
    }

    void App::backgroundWorker()
    {
        for(;;) {
            tasksForBackgroundThreads.dequeue()->execute();
        }
    }

    void App::sendToMainThread(Task::ptr task)
    {
        tasksForMainThread.enqueue(std::move(task));
    }

    void App::sendToBackgroundThreads(Task::ptr task)
    {
        tasksForBackgroundThreads.enqueue(std::move(task));
    }




}
