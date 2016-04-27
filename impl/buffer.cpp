/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Restful Inter Process Communication (Ripc) project. 
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#include <stdlib.h>
#include <string.h>
#include "errorhandling.h"
#include "buffer.h"

namespace org_restfulipc
{

    Buffer::Buffer() :
        _size(0),
        _capacity(0),
        _data(0)
    {
    }

    Buffer::Buffer(Buffer&& other)
    {
        memcpy(this, &other, sizeof (Buffer));
        memset(&other, 0, sizeof (Buffer));
    }

    Buffer& Buffer::operator=(Buffer&& other)
    {
        if (_data) {
            delete _data;
        }
        memcpy(this, &other, sizeof (Buffer));
        memset(&other, 0, sizeof (Buffer));
        return *this;
    }

    Buffer::~Buffer()
    {
        if (_data) {
            free(_data);
        }
    }

    Buffer& Buffer::write(const char* string)
    {
        size_t len = strlen(string);
        ensureCapacity(len);
        strncpy(_data + _size, string, len + 1);
        _size += len;
        return *this;
    }

    Buffer& Buffer::write(char ch)
    {
        ensureCapacity(1);
        _data[_size++] = ch;
        _data[_size] = '\0';
        return *this;
    }

    Buffer& Buffer::write(double d)
    {
        ensureCapacity(25);
        _size += snprintf(_data + _size, 25, "%.17g", d);
        return *this;
    }

    Buffer& Buffer::write(int i)
    {
        ensureCapacity(10);
        _size += snprintf(_data + _size, 10, "%d", i);
        return *this;
    }

    void Buffer::clear()
    {
        _size = 0;
    }

    bool Buffer::operator==(Buffer& other)
    {
        return _size == other._size && strcmp(data(), other.data()) == 0;
    }

    const char* Buffer::data()
    {
        return _data ? _data : "";
    }

    int Buffer::size()
    {
        return _size;
    }

    void Buffer::ensureCapacity(int numChars)
    {
        /*
         * _data is a zero-terminated string. _size does not count the terminating zero, 
         * so we need _size + numChars + 1
         */
        int needed = _size + numChars + 1;
        if (needed <= _capacity) {
            return;
        }

        if (_capacity == 0) {
            _capacity = 128;
        }

        while (_size + numChars + 1 > _capacity) _capacity *= 2;

        _data = (char*) realloc(_data, _capacity);

        if (!_data) throw C_Error();
    }

    Buffer& operator<<(Buffer& buffer, const char* str)
    {
        return buffer.write(str);
    }

    Buffer& operator<<(Buffer& buffer, const char ch)
    {
        return buffer.write(ch);
    }

    Buffer& operator<<(Buffer& buffer, double d)
    {
        return buffer.write(d);
    }

    Buffer& operator<<(Buffer& buffer, int i)
    {
        return buffer.write(i);
    }

}
