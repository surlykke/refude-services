#ifndef BUFFER_H
#define BUFFER_H

namespace org_restfulipc
{
    struct Buffer
    {
        char* data;
        int used;
        int capacity;

        Buffer(int initialCapacity = 1024);
        ~Buffer();
        void write(char* string);
        void copy(char* string);
        void clear();
        void ensureCapacity(int numChars);
    };

}
#endif // BUFFER_H
