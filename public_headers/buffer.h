#ifndef BUFFER_H
#define BUFFER_H

namespace org_restfulipc
{
    struct Buffer
    {
        Buffer(int initialCapacity = 0);
        Buffer(Buffer& other) = delete;
        Buffer(Buffer&& other);

        Buffer& operator=(Buffer& other) = delete;
        Buffer& operator=(Buffer&& other);

        ~Buffer();
        void write(const char* string);
        void write(char ch);
        void write(double d);
        void write(int i); 
        void ensureCapacity(int numChars);
        void clear();

        char* data;
        int used;
        int capacity;

        bool operator==(Buffer& other);
    };

}
#endif // BUFFER_H
