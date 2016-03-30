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
        FilteringJsonWriter(Json& json, map<string, string>* replacements, string marker = "@@");
        virtual ~FilteringJsonWriter();

        map<string, string>* replacements;
        string marker;

    protected:
        virtual void writeString(const char *string);
    };
 

}

#endif // JSONWRITER_H
