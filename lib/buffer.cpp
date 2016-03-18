#include <stdlib.h>
#include <string.h>
#include "errorhandling.h"
#include "buffer.h"

namespace org_restfulipc
{
    Buffer::Buffer(int initialCapacity) :
        used(0),
        capacity(initialCapacity),
        data(0)
    {
        if (initialCapacity > 0)  {
            data = (char*) malloc(initialCapacity);
            if (!data) throw C_Error();
        }
    }

    Buffer::Buffer(Buffer&& other)
    {
        memcpy(this, &other, sizeof(Buffer));
        memset(&other, 0, sizeof(Buffer));
    }

    Buffer& Buffer::operator=(Buffer&& other)
    {
        if (data) {
            delete data;
        }
        memcpy(this, &other, sizeof(Buffer));
        memset(&other, 0, sizeof(Buffer));
        return *this;
    }

    Buffer::~Buffer()
    {
        if (data) {
            free(data);
        }
    }

    void Buffer::write(const char* string)
    {
        size_t len = strlen(string);
        ensureCapacity(len + 1);
        strncpy(data + used, string, len + 1);
        used += len;
    }

    void Buffer::write(char ch)
    {
        ensureCapacity(2);
        data[used++] = ch;
        data[used] = '\0';
    }

    void Buffer::write(double d)
    {
        ensureCapacity(25);
        used += sprintf(data + used, "%.17g", d);
    }

    void Buffer::write(int i) {
        ensureCapacity(10);
        used += sprintf(data + used, "%d", i);
    }
    
    void Buffer::ensureCapacity(int numChars)
    {
        if (used + numChars > capacity) {
            if (capacity == 0) {
                capacity = 1;
            }
            do {
                capacity *= 2;
            }
            while (used + numChars > capacity);
           
            data = (char*) realloc(data, capacity);
            if (! data) throw C_Error();
        }
    }

    void Buffer::clear()
    {
        used = 0;
    }

    bool Buffer::operator==(Buffer& other)
    {
        return used == other.used &&
               ( used == 0 || strcmp(data, other.data) == 0);
    }

}
