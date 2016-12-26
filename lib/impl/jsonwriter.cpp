/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
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

namespace refude
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
                buffer.writeStr(", ");
            }
            writeString(key);
            buffer.writeStr(": ");
            write(value);
            written++;
        }
    }

    void JsonWriter::write(Json& json)
    {
        if (json.mType == JsonType::Object) {
            buffer.writeChr('{');
            int written = 0;
            json.eachEntry([&written, this](const char* key, Json& value) {
                writeKeyValue(written, key, value);
            });
            buffer.writeChr('}');
        }
        else if (json.mType == JsonType::Array) {
            buffer.writeChr('[');
            for (int i = 0; i < json.elements->size(); i++) {
                if (i) {
                    buffer.writeStr(", ");
                }
                write(json.elements->at(i));
            }
            buffer.writeChr(']');
        }
        else if (json.mType == JsonType::String) {
            writeString(json.str);
        }
        else if (json.mType == JsonType::Number) {
                buffer.writeDouble(json.number);
        }
        else if (json.mType == JsonType::Boolean) {
                json.boolean ? buffer.writeStr("true") : buffer.writeStr("false");
        }
        else if (json.mType == JsonType::Null) {
                buffer.writeStr("null");
        }
        else if (json.mType == JsonType::Undefined) {
            buffer.writeStr("undefined");
        }
    }

    void JsonWriter::writeString(const char* string)
    {
        buffer.writeChr('"');
        const char* c = string;
        while (*c) {
            writeChar(*(c++));
        }
        buffer.writeChr('"');
    }

    void JsonWriter::writeChar(const char chr)
    {
        if (0 <= chr && chr < 32) {
            buffer.writeStr(escapeStrings[chr]);
        }
        else if (chr == '"') {
            buffer.writeStr("\\\"");
        }
        else if (chr == '\\') {
            buffer.writeStr("\\\\");
        }
        else {
            buffer.writeChr(chr);
        }
    }


}

