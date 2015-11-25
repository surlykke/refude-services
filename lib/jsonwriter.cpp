#include <unistd.h>

#include "jsonwriter.h"

namespace org_restfulipc
{


    JsonWriter::JsonWriter(Json *json, int bufferSize, int fd) :
        json(json),
        buffer(0),
        used(0),
        size(bufferSize),
        fd(fd)
    {
        if (size < 1024) {
            size = 1024;
        }
        buffer = new char[size];
    }

    JsonWriter::~JsonWriter()
    {
        delete buffer;
    }

    void JsonWriter::write()
    {
        write(json);
        flush();
    }

    void JsonWriter::write(Json *json)
    {
       if (json->mType == JsonType::Object) {
           write("{");
           writeEntries(json->firstEntry);
           write("}");
       }
       else if (json->mType == JsonType::Array) {
           write("[");
           writeElements(json->firstElement);
           write("]");
       }
       else if (json->mType == JsonType::String) {
           writeString(json->string);
       }
       else if (json->mType == JsonType::Double) {
            write(json->numberD);
       }
       else if (json->mType == JsonType::Long) {
            write(json->numberL);
       }
       else if (json->mType == JsonType::Boolean) {
            json->boolean ? write("true") : write("false");
       }
       else if (json->mType == JsonType::Null) {
            write("null");
       }
    }

    void JsonWriter::writeElements(Element* elements)
    {
        if (elements) {
            write(elements);
            while (elements = elements->next) {
                write(", ");
                write(elements);
            }
        }
    }

    void JsonWriter::writeEntries(Entry* entries)
    {
        if (entries) {
            write(entries->key);
            write(": ");
            write(entries);
            while (entries = entries->next) {
                write(", ");
                write(entries->key);
                write(": ");
                write(entries);
            }
        }
    }

    void JsonWriter::writeString(const char* string)
    {
        write('"');
        for (const char *c = string; *c; c++) {
            write(*c); // FIXME
        }
        write('"');
    }

    void JsonWriter::write(const char *string)
    {
        for (const char* c = string; *c; c++) {
            write(*c);
        }
    }

    void JsonWriter::write(double d)
    {
        if (used > size - 25) {
            flush();
        }
        used += sprintf(buffer + used, "%E", d);
    }

    void JsonWriter::write(long l)
    {
        if (used > size - 25) {
            flush();
        }
        used += sprintf(buffer + used, "%d", l);
    }

    void JsonWriter::write(char ch)
    {
        if (used >= size) {
            flush();
        }

        buffer[used++] = ch;
    }

    void JsonWriter::flush()
    {
        int n = 0;
        int m = 0;
        while (n < used) {
            m = ::write(fd, buffer + n, used - n);
            if (m < 0) {
                throw C_Error();
            }
            n += m;
        }
        used = 0;
    }


}

