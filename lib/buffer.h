#ifndef BUFFER_H
#define BUFFER_H

namespace org_restfulipc
{
    struct Buffer
    {
        Buffer(int initialCapacity = 1024);
        ~Buffer();
        void write(const char* string);
        void write(char ch);
        void write(double d);
        void ensureCapacity(int numChars);

        char* data;
        int used;
        int capacity;
    };

}
#endif // BUFFER_H
