/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <sstream> 
#include "jsonreader.h"
#include "json.h"
#include "stacktrace.h"

namespace org_restfulipc
{

    const char* Json::typeAsString() const {
        return typeAsString(mType);
    }

    const char* Json::typeAsString(const JsonType type) const
    {
        static const char* typeNames[] = { 
            "Undefined", 
            "Object", 
            "Array", 
            "String", 
            "Number"
            "Boolean", 
            "Null" 
        };
        return typeNames[(uint8_t)type];
    }

    bool Json::undefined() {
        return mType == JsonType::Undefined;
    }

    JsonType Json::type()
    {
        return mType;
    }


    void Json::deleteChildren()
    {
        if (mType == JsonType::Object){
            delete entries;
        }
        else if (mType == JsonType::Array) {
            delete elements;
        }
        else if (mType == JsonType::String) {
            delete str;
        }
    }

    Json::Json(Json&& other)
    {
        memcpy(this, &other, sizeof(Json));
        memset(&other, 0, sizeof(Json));
    }


    Json::Json(JsonConst jsonConst)
    {
        switch (jsonConst) {
        case JsonConst::EmptyObject:
            mType = JsonType::Object;
            entries = new Map<Json>();
            break;
        case JsonConst::EmptyArray:
            mType = JsonType::Array;
            elements = new std::vector<Json>();
            break;
        case JsonConst::TRUE:
            mType = JsonType::Boolean;
            boolean = true;
            break;
        case JsonConst::FALSE:
            mType = JsonType::Boolean;
            boolean = false;
            break;
        case JsonConst::Null:
            mType = JsonType::Null;
            break;
        }
    }
        
    Json::Json(const char* string) :
        mType(JsonType::String), 
        str(strdup(string))
    {
    }

    Json::Json(std::string string) :
        mType(JsonType::String),
        str(strdup(string.data()))
    {
    } 
        
    Json::Json(double number) :
        mType(JsonType::Number), 
        number(number) 
    {
    }
    
 
    Json::~Json()
    {
        deleteChildren();
    }

    Json Json::deepCopy()
    {
        Json copy;
        memcpy(&copy, this, sizeof(Json));
        if (mType == JsonType::Array) {
            for (size_t i = 0; i < elements->size(); i++) {
                copy.append((*elements)[i].deepCopy());
            }
        }
        else if (mType == JsonType::Object) {
            for (size_t i = 0; i < entries->size(); i++) {
                copy.append(strdup(entries->list[i].key), entries->list[i].value.deepCopy());
            }
            entries->sorted = entries->list.size();
        }

        return copy;
    }

    Json& Json::operator=(Json&& other)
    {
        deleteChildren();
        memcpy(this, &other, sizeof(Json));
        memset(&other, 0, sizeof(Json));
        return *this;
    }



    Json&Json::operator=(JsonConst jsonConst)
    {
        deleteChildren();
        switch (jsonConst) {
        case JsonConst::EmptyObject:
            mType = JsonType::Object;
            entries = new Map<Json>();
            break;
        case JsonConst::EmptyArray:
            mType = JsonType::Array;
            elements = new std::vector<Json>();
            break;
        case JsonConst::TRUE:
            mType = JsonType::Boolean;
            boolean = true;
            break;
        case JsonConst::FALSE:
            mType = JsonType::Boolean;
            boolean = false;
            break;
        case JsonConst::Null:
            mType = JsonType::Null;
            break;
        }
        return *this;
    }

    bool Json::operator==(const Json& other) const
    {
        if (other.mType != mType) {
            return false;
        }
        switch (mType) {
        case JsonType::Undefined: return false;
        case JsonType::String: return strcmp(str, other.str) == 0;
        case JsonType::Number: return number == other.number;
        case JsonType::Boolean: return boolean == other.boolean;
        case JsonType::Object: 
            if (size() != other.size()) {
                return false;
            }
            for (int i = 0; i < size(); i++) {
                if (strcmp(entries->at(i).key, other.entries->at(i).key) ||
                    entries->at(i).value.operator!=(other.entries->at(i).value)) {
                    return false;
                }
            }
            return true;
        case JsonType::Array: 
            for (int i = 0; i < size(); i++) {
                if (elements->at(i) != other.elements->at(i)) {
                    return false;
                }
            }
            return true;
        case JsonType::Null:
            return true;
        }
    }

    bool Json::operator!=(const Json& other) const
    {
        return !operator==(other);
    }


    Json& Json::operator[](const char *index)
    {
        typeAssert("operator[const char*]", JsonType::Object);
        return (*entries)[index];
    }

    Json& Json::operator[](std::string index)
    {
        return operator[](index.data());
    }

    Json& Json::operator[](int index) const
    {
        typeAssert("operator[int]", JsonType::Array);
        return (*elements)[index];
    }

    Json Json::take(int index)
    {
        typeAssert("take(int)", JsonType::Array);
        Json tmp = std::move((*elements)[index]);
        elements->erase(elements->begin() + index);
        return tmp;
    }


    bool Json::contains(const char* key) const
    {
        typeAssert("contains(const char*)", JsonType::Object);
        return entries->find(key) > -1;
    }

    const char* Json::keyAt(size_t index) const
    {
        typeAssert("keyAt(int)", JsonType::Object);
        if (index >= entries->size()) {
            throw RuntimeError("Index %d out of range (size: %d)", index, size());
        }
        return entries->list[index].key;
    }


    uint Json::size() const
    {
        if (mType == JsonType::Array) {
            return elements->size();
        }
        else if (mType == JsonType::Object) {
            return entries->size();
        }
        else {
            throw RuntimeError("size() can only be called on Objects or Arrays");
        }
    }

    Json& Json::append(Json&& json)
    {
        typeAssert("append(Json&&)", JsonType::Array);
        elements->push_back(std::move(json));
        return elements->back();
    }

    Json& Json::append(const char* key, Json&& json) {
        typeAssert("append(const char*, Json&&)", JsonType::Object);
        return entries->add(key, std::move(json));
    }



    Json& Json::insertAt(int index, Json&& json)
    {
        typeAssert("insertAt(index, Json&&)", JsonType::Array);
        elements->insert(elements->begin() + index, std::move(json));
        return elements->at(index);
    }

    void Json::typeAssert(const char* operation, JsonType type) const {
        if (type != mType) {
//            print_stacktrace();
            throw RuntimeError("Attempting %s on json of type %s", operation, typeAsString());
        }
    }

    Json Json::take(const char* key)
    {
        typeAssert("take(const char*)", JsonType::Object);
        return entries->take(key);
    }


    org_restfulipc::Json::operator bool() const
    {
        typeAssert("operator bool()", JsonType::Boolean);
        return boolean;
    }

    org_restfulipc::Json::operator long() const
    {
        typeAssert("operator long()", JsonType::Number);
        return (long)number;
    }

    org_restfulipc::Json::operator double() const
    {
        typeAssert("operator double()", JsonType::Number);
        return number;
    }

    org_restfulipc::Json::operator const char *() const
    {
        typeAssert("operator const char*()", JsonType::String);
        return str;
    }

    Json& operator<<(Json& json, const char* serialized)
    {

        JsonReader reader(serialized);
        json = reader.read();
        return json;
    }


}
