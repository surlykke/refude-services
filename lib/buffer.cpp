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

    void Buffer::write(char* string)
    {
        size_t len = strlen(string);
        ensureCapacity(len + 1);
        strcpy(data + used, string);
        used += len;

    }

    void Buffer::copy(char* string)
    {
        write(string);
        used++;
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
