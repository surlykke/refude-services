#ifndef JSONWRITER_H
#define JSONWRITER_H
#include "json.h"

namespace org_restfulipc
{

    class JsonWriter
    {
    public:
        JsonWriter(Json* json, int bufferSize = 1024, int fd = 1);
        ~JsonWriter();
        void write();

    private:
        void write(Json* json);
        void writeElements(Element* elements);
        void writeEntries(Entry* entries);

        void writeString(const char *string);
        void write(const char* string);
        void write(double d);
        void write(long l);
        void write(char ch);
        void flush();

        Json* json;

        char *buffer;
        int used;
        int size;

        int   fd;
    };
}

#endif // JSONWRITER_H
