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
#include "fd.h"
#include "buffer.h"

namespace refude
{

    Buffer::Buffer() :
        _data(0),
        _size(0),
        _capacity(0)
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

    Buffer& Buffer::writeStr(const char* string)
    {
        return writeStrn(string, strlen(string));
    }

    Buffer& Buffer::writeStrn(const char* string, size_t n)
    {
        ensureCapacity(n);
        strncpy(_data + _size, string, n);
        _size += n;
        _data[_size] = '\0';
        return *this;
    }

    
    Buffer& Buffer::writeChr(char ch)
    {
        ensureCapacity(1);
        _data[_size++] = ch;
        _data[_size] = '\0';
        return *this;
    }

    Buffer& Buffer::writeDouble(double d)
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

    Buffer& Buffer::writeLong(long i)
    {
        ensureCapacity(10);
        _size += snprintf(_data + _size, 10, "%ld", i);
        return *this;
    }


    Buffer& Buffer::writeFile(const char* path)
    {
        Fd fd = open(path, O_RDONLY | O_CLOEXEC);
        writeFile(fd.fd);
    }

    Buffer& Buffer::writeFile(int fd)
    {
        for (;;) {
            ensureCapacity(1025);
            ssize_t bytesRead = read(fd, _data + _size, 1024);
            if (bytesRead < 0) throw C_Error();
            _size += bytesRead;
            if (bytesRead < 1024) {
                break;
            }
        }
        _data[_size] = '\0';
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
        Fd fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
        int bytesWritten = 0;
        int bytesWrittenTotal = 0;
        while ((bytesWritten = ::write(fd.fd, _data + bytesWrittenTotal, _size - bytesWrittenTotal)) > 0) {
            bytesWrittenTotal += bytesWritten;
        }
        if (bytesWritten < 0) throw C_Error();
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
}
