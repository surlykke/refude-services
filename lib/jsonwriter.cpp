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
        write('\0');
    }

    JsonWriter::~JsonWriter()
    {
    }

    void JsonWriter::write(const Json* json)
    {
        if (json->mType == JsonType::Object) {
            write("{");
            if (json->entries->size() > 0) {
                writeString(json->entries->at(0).key);
                write(":");
                write(&(json->entries->at(0).value));
                
                for (int i = 1; i < json->entries->size(); i++) {
                    write(",");
                    writeString(json->entries->at(i).key);
                    write(":");
                    write(&(json->entries->at(i).value));
                }
            }
            write("}");
        }
        else if (json->mType == JsonType::Array) {
            write("[");
            if (json->elements->size() > 0) {
                write(&(json->elements->at(0))); 
                for (int i = 1; i < json->elements->size(); i++) {
                    write(", ");
                    write(&(json->elements->at(i)));
                }
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
        else if (json->mType == JsonType::Undefined) {
            write("undefined");
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

