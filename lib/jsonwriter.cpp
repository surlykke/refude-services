#include <unistd.h>
#include "errorhandling.h"
#include "json.h"
#include "jsonwriter.h"

namespace org_restfulipc
{


    JsonWriter::JsonWriter(Json *json) :
        json(json),
        buffer()
    {
        if (!json) throw RuntimeError("JsonWriter cannot handle NULL json ptr");
        write(json);
    }

    JsonWriter::~JsonWriter()
    {
    }

    void JsonWriter::write(Json *json)
    {
       if (json->mType == JsonType::Object) {
           write("{");
           const char* separator = "";
           for (int i = 0; i < json->entries->size; i++) {
               write(separator);
               writeString(json->entries->data[i].key);
               write(":");
               write(&(json->entries->data[i].value));
               separator = ",";
           }
           write("}");
       }
       else if (json->mType == JsonType::Array) {
           write("[");
           const char* separator = "";
           for (int i = 0; i < json->elements->size; i++) {
                write(separator);
                write(&(json->elements->data[i]));
                separator = ",";
           }
           write("]");
       }
       else if (json->mType == JsonType::String) {
           writeString(json->string);
       }
       else if (json->mType == JsonType::Number) {
            write(json->number);
       }
       else if (json->mType == JsonType::Boolean) {
            json->boolean ? write("true") : write("false");
       }
       else if (json->mType == JsonType::Null) {
            write("null");
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
        buffer.ensureCapacity(25);
        buffer.used += sprintf(buffer.data + buffer.used, "%.17g", d);
    }

    void JsonWriter::write(char ch)
    {
        buffer.ensureCapacity(1);
        buffer.data[buffer.used++] = ch;
    }

}

