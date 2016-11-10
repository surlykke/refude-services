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
    /**
     * Basically a wrapper for char* string. Grows the string as needed
     * 
     * Used for medium to huge strings - primarily
     * serialization of json objects, so we start out with not-so-small 128 bytes, and do 
     * no short-string optimizations
     * There is no copy semantics, to make sure we do not accidentically make performance heavy 
     * copies. To copy a buffer, do:
     * 
     *    Buffer buffer;
     *    Buffer bufferCopy;
     *    .
     *    .
     *    bufferCopy.write(buffer.data());
     * 
     */
    class Buffer
    {
    public:
        static Buffer fromFile(const char* filePath);
        Buffer();
        Buffer(Buffer& other) = delete;
        Buffer(Buffer&& other);

        Buffer& operator=(Buffer& other) = delete;
        Buffer& operator=(Buffer&& other);

        ~Buffer();
        Buffer& write(const char* string);
        Buffer& writen(const char* string, size_t n);
        Buffer& write(char ch);
        Buffer& write(double d);
        Buffer& write(int i); 

        void toFile(const char* filePath);
        void clear();

        bool operator==(Buffer& other);

        const char* data();
        int size();

    private:
        void ensureCapacity(int numChars);
        char* _data;
        int _size;
        int _capacity;

    };

    Buffer& operator<<(Buffer& buffer, const char* str);
    Buffer& operator<<(Buffer& buffer, const char ch);
    Buffer& operator<<(Buffer& buffer, double d);
    Buffer& operator<<(Buffer& buffer, int i);
}
#endif // BUFFER_H
