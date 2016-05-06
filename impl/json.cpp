/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <stdio.h>
#include <stdarg.h>
#include <sstream> 
#include "jsonreader.h"
#include "json.h"

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
                if (strcmp(entries->keyAt(i), other.entries->keyAt(i)) ||
                    entries->valueAt(i) != other.entries->valueAt(i)) {
                    return false;
                }
            }
            return true;
        case JsonType::Array: 
            if (size() != other.size()) {
                return false;
            }
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
        typeAssert(JsonType::Object, "operator[\"%s\"]", index);
        return (*entries)[index];
    }

    Json& Json::operator[](std::string index)
    {
        return operator[](index.data());
    }

    Json& Json::operator[](int index) const
    {
        typeAssert(JsonType::Array, "operator[%d]", index);
        return (*elements)[index];
    }

    vector<const char*> Json::keys() const
    {
        typeAssert(JsonType::Object, "keys()");
        return entries->keys();
    }


    Json Json::take(int index)
    {
        typeAssert(JsonType::Array, "take(%d)", index);
        Json tmp = std::move((*elements)[index]);
        elements->erase(elements->begin() + index);
        return tmp;
    }


    bool Json::contains(const char* key) const
    {
        typeAssert(JsonType::Object, "contains(\"%s\")", key);
        return entries->find(key) > -1;
    }

    bool Json::contains(std::string key) const
    {
        return  contains(key.data());
    }

    int Json::find(const char* str) const
    {
        typeAssert(JsonType::Array, "find(\"%s\"", str);
        for (int i = 0; i < elements->size(); i++) {
            if ((*elements)[i].mType == JsonType::Array && !strcmp(str, (*elements)[i].str)) {
                return i;
            }
        }
        
        return -1;
    }

    int Json::find(std::string str) const
    {
        return find(str.data());
    }

    const char* Json::keyAt(size_t index) const
    {
        typeAssert(JsonType::Object, "keyAt(%d)", index);
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
            throw RuntimeError("size() called on %s - it can only be called on Objects or Arrays", typeAsString());
        }
    }

    Json& Json::append(Json&& json)
    {
        typeAssert(JsonType::Array, "append(Json&&)");
        elements->push_back(std::move(json));
        return elements->back();
    }

    Json& Json::append(const char* key, Json&& json) {
        typeAssert(JsonType::Object, "append(\"%s\", Json&&)", key);
        return entries->add(key, std::move(json));
    }



    Json& Json::insertAt(int index, Json&& json)
    {
        typeAssert(JsonType::Array, "insertAt(%d, Json&&)", index);
        elements->insert(elements->begin() + index, std::move(json));
        return elements->at(index);
    }

    Json Json::take(const char* key)
    {
        typeAssert(JsonType::Object, "take(\"%s\")", key);
        return entries->take(key);
    }


    org_restfulipc::Json::operator bool() const
    {
        typeAssert(JsonType::Boolean, "operator bool()");
        return boolean;
    }

    org_restfulipc::Json::operator long() const
    {
        typeAssert(JsonType::Number, "operator long()");
        return (long)number;
    }

    org_restfulipc::Json::operator double() const
    {
        typeAssert(JsonType::Number, "operator double()");
        return number;
    }

    org_restfulipc::Json::operator const char *() const
    {
        typeAssert(JsonType::String, "operator const char*()");
        return str;
    }

    Json& operator<<(Json& json, const char* serialized)
    {

        JsonReader reader(serialized);
        json = reader.read();
        return json;
    }

    void Json::typeAssert(JsonType otherType, const char* operationDescFmt, ...) const {
        if (otherType != mType) {
            char operatorDescription[1024];
            va_list args;
            va_start(args, operationDescFmt);
            vsprintf(operatorDescription, operationDescFmt, args);
            va_end(args);
            throw RuntimeError("Attempting '%s' on json of type '%s'. Type should have been '%s'", 
                               operatorDescription,
                               typeAsString(),
                               typeAsString(otherType));
        }
    }


}
