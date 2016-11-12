/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <unistd.h>
#include "errorhandling.h"
#include "json.h"
#include "jsonwriter.h"

const char* escapeStrings[32] = 
    {   
        "\u0000", "\u0001", "\u0002", "\u0003", "\u0004", "\u0005", "\u0006", "\u0007", 
        "\u0008", "\u0009", "\u000a", "\u000b", "\u000c", "\u000d", "\u000e", "\u000f", 
        "\u0010", "\u0011", "\u0012", "\u0013", "\u0014", "\u0015", "\u0016", "\u0017",
        "\u0018", "\u0019", "\u001a", "\u001b", "\u001c", "\u001d", "\u001e", "\u001f" 
    };

namespace org_restfulipc
{

    JsonWriter::JsonWriter(Json& json) :
        buffer() 
    {
        write(json);
    }

    JsonWriter::~JsonWriter()
    {
    }
    
    JsonWriter::JsonWriter() :
        buffer()
    {
    }

    void JsonWriter::writeObject(Json& json){
    }

    void JsonWriter::writeKeyValue(int& written, const char* key, Json& value)
    {
        if (!value.undefined()) {
            if (written) {
                buffer.write(", ");
            }
            writeString(key);
            buffer.write(": ");
            write(value);
            written++;
        }
    }

    void JsonWriter::write(Json& json)
    {
        if (json.mType == JsonType::Object) {
            buffer.write('{');
            int written = 0; 
            for (int i = 0; i < json.entries->size(); i++) {
                writeKeyValue(written, json.entries->keyAt(i), json.entries->valueAt(i));
            }
            buffer.write('}');
        }
        else if (json.mType == JsonType::Array) {
            buffer.write('[');
            for (int i = 0; i < json.elements->size(); i++) {
                if (i) {
                    buffer.write(", ");
                }
                write(json.elements->at(i));
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
        const char* c = string;
        while (*c) {
            writeChar(*(c++));
        }
        buffer.write('"');
    }

    void JsonWriter::writeChar(const char chr)
    {
        if (0 <= chr && chr < 32) {
            buffer.write(escapeStrings[chr]);
        }
        else if (chr == '"') {
            buffer.write("\\\"");
        }
        else if (chr == '\\') {
            buffer.write("\\\\");
        }
        else {
            buffer.write(chr); 
        }
    }


}

