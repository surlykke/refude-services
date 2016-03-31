/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

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
