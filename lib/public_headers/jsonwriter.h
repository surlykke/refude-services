/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LGPL21 file for a copy of the license.
*/

#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <map>
#include <string>
#include <vector>

#include "buffer.h"

namespace refude
{
    class Json;

    struct JsonWriter
    {
        JsonWriter(Json& json);
        ~JsonWriter();

        Buffer buffer;

    protected:
        JsonWriter();
        void write(Json& json);
        void writeObject(Json& json);
        virtual void writeKeyValue(int& written, const char* key, Json& value);
        void writeString(const char *string);
        void writeChar(const char chr);
    };

}

#endif // JSONWRITER_H
