#include <unistd.h>
#include "errorhandling.h"
#include "json.h"
#include "jsonwriter.h"

namespace org_restfulipc
{


    JsonWriter::JsonWriter(const Json& json) :
        buffer(128)
    {
        write(json);
    }

    JsonWriter::~JsonWriter()
    {
    }
    
    JsonWriter::JsonWriter() :
        buffer(128)
    {
    }

    void JsonWriter::write(const Json& json)
    {
        if (json.mType == JsonType::Object) {
            buffer.write('{');
            if (json.entries->size() > 0) {
                writeString(json.entries->at(0).key);
                buffer.write(": ");
                write(json.entries->at(0).value);
                
                for (int i = 1; i < json.entries->size(); i++) {
                    buffer.write(", ");
                    writeString(json.entries->at(i).key);
                    buffer.write(": ");
                    write(json.entries->at(i).value);
                }
            }
            buffer.write('}');
        }
        else if (json.mType == JsonType::Array) {
            buffer.write('[');
            if (json.elements->size() > 0) {
                write(json.elements->at(0)); 
                for (int i = 1; i < json.elements->size(); i++) {
                    buffer.write(", ");
                    write(json.elements->at(i));
                }
            }
            buffer.write(']');
        }
        else if (json.mType == JsonType::String) {
            writeString(json.str);
        }
        else if (json.mType == JsonType::Number) {
                buffer.write(json.number);
        }
        else if (json.mType == JsonType::Boolean) {
                json.boolean ? buffer.write("true") : buffer.write("false");
        }
        else if (json.mType == JsonType::Null) {
                buffer.write("null");
        }
        else if (json.mType == JsonType::Undefined) {
            buffer.write("undefined");
        }
    }

    void JsonWriter::writeString(const char* string)
    {
        buffer.write('"');
        for (const char *c = string; *c; c++) {
            buffer.write(*c); // FIXME
        }
        buffer.write('"');
    }

    FilteringJsonWriter::FilteringJsonWriter(Json& json, map<string, string>* replacements, string marker) :
        JsonWriter(),
        replacements(replacements),
        marker(marker)
    {
        write(json);
    }

    FilteringJsonWriter::~FilteringJsonWriter()
    {
    }

    void FilteringJsonWriter::writeString(const char* str)
    {
        if (! strncmp(marker.data(), str, marker.size())) {
            str = (*replacements)[str].data();
        }
        buffer.write('"');
        for (const char *c = str; *c; c++) {
            buffer.write(*c); // FIXME
        }
        buffer.write('"');
    }
}

