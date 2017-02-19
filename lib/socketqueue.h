#ifndef SOCKET_QUEUE_H
#define SOCKET_QUEUE_H
#include <thread>
#include <functional>
#include <memory>
#include <tuple>
#include <experimental/tuple>
#include <vector>
#include "queue.h"

namespace refude
{
    class SocketQueue
    {
    public:
        SocketQueue();
        virtual ~SocketQueue();

        void push(int socket);
        int pop();

    private:
        int sockets[32];
        int numSockets;
        std::mutex mutex;
        std::condition_variable notFull;
        std::condition_variable notEmpty;

    };
}
#endif // SOCKET_QUEUE_H
