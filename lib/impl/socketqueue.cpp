#include <tuple>
#include "errorhandling.h"
#include "socketqueue.h"

namespace refude
{
    SocketQueue::SocketQueue() :
        numSockets(0),
        mutex(),
        notFull(),
        notEmpty()
    {
    }

    SocketQueue::~SocketQueue()
    {
    }


    void SocketQueue::push(int socket)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (numSockets == 32) notEmpty.wait(lock);
            sockets[numSockets++] = socket;
        }

         notEmpty.notify_one();
    }

    int SocketQueue::pop()
    {
        int dequeuedSocket = -1;
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (numSockets == 0) {
                notEmpty.wait(lock);
            }
            dequeuedSocket = sockets[0];
            for (int i = 1; i < numSockets; i++) {
                sockets[i - 1] = sockets[i];
            }
            numSockets--;
        }
        notFull.notify_one();
        return dequeuedSocket;
    }
}
