#ifndef SHORTQUEUE
#define SHORTQUEUE

#include <mutex>
#include <condition_variable>
#include <iostream>
using namespace std;

namespace org_restfulipc
{
    template<int size> struct ShortMtQueue
    {

        ShortMtQueue() : elements(), count(0), m() { std::cout << "ShortMtQueue constructor\n";}

        void enqueue(int s) {
           {
                unique_lock<mutex> lock(m);

                while (count >= size) {
                    notFull.wait(lock);
                }

                for (int i = count; i > 0; i--) {
                    elements[i] = elements[i - 1];
                }

                elements[0] = s;
                count++;
            }

            notEmpty.notify_one();
        }

        int dequeue() {
            int result;
            {
                unique_lock<mutex> lock(m);

                while (count <= 0) {
                    notEmpty.wait(lock);
                }

                result = elements[--count];
            }

            notFull.notify_one();

            return result;
        }

        int elements[size];
        int count;

        mutex m;
        condition_variable notFull;
        condition_variable notEmpty;
    };
}

#endif // SHORTQUEUE

