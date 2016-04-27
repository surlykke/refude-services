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

    struct JsonWriter
    {
        JsonWriter(Json& json);
        ~JsonWriter();

        Buffer buffer;

    protected:
        JsonWriter();
        void write(Json& json);
        virtual void writeObject(Json& json);

        virtual void writeString(const char *string);
    };

    struct LocalizingJsonWriter : public JsonWriter
    {
        LocalizingJsonWriter(Json& json, string locale, const char* lastResort = "");
        virtual ~LocalizingJsonWriter();
    
    protected:
        void writeObject(Json& json) override;
    
    private:
        const string locale;
        const char* lastResort;
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
