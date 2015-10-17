/* 
 * File:   json.cpp
 * Author: christian
 * 
 * Created on 23. juli 2015, 10:32
 */

#include <stdlib.h>

#include "json.h"
#include "errorhandling.h"

namespace org_restfulipc
{

    const char* tag2str(JsonTypeTag tag) {
        static const char* names[] = 
        { 
            "Number",
            "Boolean",
            "String",
            "Object",
            "Array"
        };

        return names[(int) tag];
    }
 


    int AbstractJson::drySerialize() {
        switch (typeTag) {
        case JsonTypeTag::String: return jsonCast<JsonString>(this)->lengthNeededToSerialize();
        case JsonTypeTag::Number: return jsonCast<JsonNumber>(this)->lengthNeededToSerialize();
        case JsonTypeTag::Boolean: return jsonCast<JsonBoolean>(this)->lengthNeededToSerialize();
        case JsonTypeTag::Array: return jsonCast<JsonArray>(this)->lengthNeededToSerialize();
        case JsonTypeTag::Object: return jsonCast<JsonObject>(this)->lengthNeededToSerialize();
        }
    }

    int AbstractJson::serialize(char* dest) {
        switch (typeTag) {
        case JsonTypeTag::String: return jsonCast<JsonString>(this)->serialize(dest);
        case JsonTypeTag::Number: return jsonCast<JsonNumber>(this)->serialize(dest);
        case JsonTypeTag::Boolean: return jsonCast<JsonBoolean>(this)->serialize(dest);
        case JsonTypeTag::Array: return jsonCast<JsonArray>(this)->serialize(dest);
        case JsonTypeTag::Object: return jsonCast<JsonObject>(this)->serialize(dest);
        }
    };
}