/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <map>
#include <string>

#include "buffer.h"

namespace org_restfulipc
{
    using namespace std;
    class Json;
    class Buffer;

    struct JsonWriter
    {
        JsonWriter(const Json& json);
        ~JsonWriter();

        Buffer buffer;

    protected:
        JsonWriter();
        void write(const Json& json);
        virtual void writeString(const char *string);
    };

    struct FilteringJsonWriter : public JsonWriter
    {
        FilteringJsonWriter(Json& json, 
                            const char* marker, 
                            Json& replacements, 
                            Json& fallbackReplacements,
                            const char* lastResort);
        virtual ~FilteringJsonWriter();

        const char* marker;
        Json& replacements;
        Json& fallbackReplacements;
        const char* lastResort;

    protected:
        virtual void writeString(const char *string);
    };
 

}

#endif // JSONWRITER_H
