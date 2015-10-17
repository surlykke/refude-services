/* 
 * File:   heap.h
 * Author: christian
 *
 * Created on 25. juli 2015, 23:18
 */

#ifndef HEAP_H
#define    HEAP_H

#include <memory>

using namespace std;

#define CHUNK 8192

namespace org_restfulipc 
{
    class Heap
    {
    public:
        Heap() : buffer((char*) malloc(CHUNK)), p(buffer), bufferSize(CHUNK), freeSize(CHUNK) {}
        virtual ~Heap() { free(buffer); }

            
        template<typename T>
        void* allocate()
        {
            if (! std::align(alignof(T), sizeof(T), p, freeSize)) {
                bufferSize += CHUNK;
                freeSize += CHUNK;
                buffer = realloc(buffer, bufferSize);
                if (! std::align(alignof(T), sizeof(T), p, freeSize)) {    
                    throw "Allocation failed";
                }
            }
        
            void *result = p;
            p = (char*)p + sizeof(T);
            freeSize -= sizeof(T);
            return result;
        }
    

    private:
        void* buffer;
        void* p;
        size_t bufferSize;
        size_t freeSize;

    };
}
#endif    /* HEAP_H */

