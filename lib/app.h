#ifndef MAIN_APP_H
#define MAIN_APP_H
#include <thread>
#include <functional>
#include <memory>
#include <tuple>
#include <experimental/tuple>
#include <vector>
#include "queue.h"

namespace refude
{

    struct Task
    {
        typedef std::unique_ptr<Task> ptr;
        virtual ~Task();
        virtual void execute() = 0;
    };

    template <typename Callable, typename ...A>
    struct Packed : public Task
    {
        Callable c;
        std::tuple<A...> t;

        Packed(Callable c, A...a) :
            Task(),
            c(c),
            t(std::forward_as_tuple(std::move(a)...))
        {
        }

        virtual void execute() override
        {
            std::experimental::apply(c, std::move(t));
        }
    };


    class App
    {
    public:
        static App& inst();
        static void run();

        void sendToMainThread(Task::ptr task);
        void sendToBackgroundThreads(Task::ptr task);

    private:
        App();
        void backgroundWorker();
        Queue<Task::ptr> tasksForMainThread;
        Queue<Task::ptr> tasksForBackgroundThreads;

        std::vector<std::thread> backgroundThreads;
    };


    template <typename Callable, typename ...A>
    std::unique_ptr<Packed<Callable, A...>> pack(Callable c, A...a)
    {
        return std::make_unique<Packed<Callable, A...>>(c, std::move(a)...);
    }

    template<typename Callable, typename...Args>
    void runOnMain(Callable c, Args...args)
    {
        App::inst().sendToMainThread(std::make_unique<Packed<Callable, Args...>>(c, std::move(args)...));
    }

    template<typename Callable, typename...Args>
    void runInBackground(Callable c, Args...args)
    {
        App::inst().sendToBackgroundThreads(std::move(std::make_unique<Packed<Callable, Args...>>(c, std::move(args)...)));
    }
}
#endif // MAIN_APP_H
