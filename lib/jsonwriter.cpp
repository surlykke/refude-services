#include <unistd.h>

#include "jsonstruct.h"
#include "jsonwriter.h"

namespace org_restfulipc
{


    JsonWriter::JsonWriter(JsonStruct *jsonStruct, int bufferSize, int fd) :
        jsonStruct(jsonStruct),
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
        write(jsonStruct->root);
        flush();
    }

    void JsonWriter::write(Json json)
    {
       if (json.type() == JsonType::Object) {
           write("{");
           writeEntries(json.firstEntry);
           write("}");
       }
       else if (json.type() == JsonType::Array) {
           write("[");
           writeElements(json.firstElement);
           write("]");
       }
       else if (json.type() == JsonType::String) {
           writeString(json.string);
       }
       else if (json.type() == JsonType::Double) {
            write(json.numberD);
       }
       else if (json.type() == JsonType::Long) {
            write(json.numberL);
       }
       else if (json.type() == JsonType::Boolean) {
            json.boolean ? write("true") : write("false");
       }
       else if (json.type() == JsonType::Null) {
            write("null");
        }
    }

    void JsonWriter::writeElements(Element* elements)
    {
        if (elements) {
            write(*elements);
            while (elements = elements->next) {
                write(", ");
                write(*elements);
            }
        }
    }

    void JsonWriter::writeEntries(Entry* entries)
    {
        if (entries) {
            write(*entries);
            while (entries = entries->next) {
                write(", ");
                write(*entries);
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

