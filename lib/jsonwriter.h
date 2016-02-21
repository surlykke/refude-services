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
        JsonWriter(Buffer* buffer);
        ~JsonWriter();
        void write(const Json& json);

        Buffer* buffer;

    private:
        virtual void writeString(const char *string);

    };

    struct FilteringJsonWriter : public JsonWriter
    {
        FilteringJsonWriter(Buffer* buffer, map<string, string>* replacements, string marker = "@@");
        virtual ~FilteringJsonWriter();

        map<string, string>* replacements;
        string marker;

    private:
        virtual void writeString(const char *string);
    };
 

}

#endif // JSONWRITER_H
