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
        data = (char*) malloc(initialCapacity);
        if (!data) throw C_Error();
    }

    Buffer::~Buffer()
    {
        free(data);
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

    void Buffer::ensureCapacity(int numChars)
    {
        if (used + numChars > capacity) {
            do {
                capacity *= 2;
            }
            while (used + numChars > capacity);
           
            data = (char*) realloc(data, capacity);
            if (! data) throw C_Error();
        }
    }
}
