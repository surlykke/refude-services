#ifndef JSONWRITER_H
#define JSONWRITER_H

#include "buffer.h"

namespace org_restfulipc
{
    class Json;

    struct JsonWriter
    {
        JsonWriter(Json* json);
        ~JsonWriter();

        Json* json;
        Buffer buffer;

    private:
        void write(Json* json);

        void writeString(const char *string);
        void write(const char* string);
        void write(double d);
        void write(char ch);

    };
}

#endif // JSONWRITER_H
