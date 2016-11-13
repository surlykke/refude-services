/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "errorhandling.h"
#include "buffer.h"

namespace refude
{

    struct FileDescriptor 
    {
        FileDescriptor(const char* path, int flags) 
        {
            _fd = open(path, flags);
            C_ERROR_IF(_fd < 0);
        }

        FileDescriptor(const char* path, int flags, int mode) 
        {
            _fd = open(path, flags, mode); 
            C_ERROR_IF(_fd < 0);
        }

        ~FileDescriptor() 
        {
            if (_fd > -1) {
                close(_fd);
            }
        }

        operator int() { return _fd; }

        int _fd;
    };

    Buffer Buffer::fromFile(const char* filePath)
    {
        Buffer buf;
        FileDescriptor fd(filePath, O_RDONLY | O_CLOEXEC);
        int bytesRead = 0;
        buf.ensureCapacity(1024);
        while((bytesRead = read(fd, buf._data, 1024)) > 0) {
            buf._size += bytesRead;
            buf._data[buf._size] = '\0';
            buf.ensureCapacity(1024);
        }
        C_ERROR_IF(bytesRead < 0);
        return buf;
    }


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
        return writen(string, strlen(string));
    }

    Buffer& Buffer::writen(const char* string, size_t n)
    {
        ensureCapacity(n);
        strncpy(_data + _size, string, n);
        _size += n;
        _data[_size] = '\0';
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
        int newSize = _size + snprintf(_data + _size, 25, "%.16g", d);
    
        // Annoyingly snprintf formats floats according to locale, which 
        // breaks json serialization, and we can't set locale from library.
        // Seemingly locale only affects the decimal point, so we band-aid it here.
        // TODO: Write better double to string converter
        for (; _size < newSize; _size++) {
            if (_data[_size] == ',') {
                _data[_size] = '.';
            }
        }

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

    void Buffer::toFile(const char* filePath)
    {
        FileDescriptor fd(filePath, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
        int bytesWritten = 0;
        int bytesWrittenTotal = 0;
        while ((bytesWritten = ::write(fd, _data + bytesWrittenTotal, _size - bytesWrittenTotal)) > 0) {
            bytesWrittenTotal += bytesWritten;
        }
        C_ERROR_IF(bytesWritten < 0);
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
