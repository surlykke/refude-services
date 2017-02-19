/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <mutex>
#include <condition_variable>


namespace refude
{
    const static int QUEUE_SIZE = 1024;
    const static int QUEUE_SIZE_MASK = 1023;


    /**
     * @brief Simple thread-safe queue. It uses a circular buffer.
     * Elements are inserted at the end, removed at the front.
     * (to the extend end and front makes sense when talking about a circular buffer...)
     */
    template <typename T>
    struct Queue
    {
        Queue() : elements(), start(0), count(0), m() { }

        void enqueue(T&& t)
        {
            {
                std::unique_lock<std::mutex> lock(m);

                while (count >= QUEUE_SIZE) {
                    notFull.wait(lock);
                }

                elements[(start + count) & QUEUE_SIZE_MASK] = std::move(t);
                count++;
                std::cout << "enqueued, count now: " << count << "\n";
            }

             notEmpty.notify_one();
         }

        T dequeue()
        {
            T result;
            {
                std::unique_lock<std::mutex> lock(m);

                while (count <= 0) {
                    notEmpty.wait(lock);
                }

                result = std::move(elements[start]);
                start = (start + 1) & QUEUE_SIZE_MASK;
                count--;
            }
            notFull.notify_one();
            std::cout << "Dequeued, count now: " << count << "\n";
            return std::move(result);
        }

        T elements[1024];
        int start;
        int count;

        std::mutex m;
        std::condition_variable notFull;
        std::condition_variable notEmpty;
    };

}

#endif // QUEUE_H
