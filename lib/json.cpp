#include <sstream>

#include "jsonreader.h"
#include "json.h"

namespace org_restfulipc
{

    const char* Json::typeAsString() {
        return typeAsString(mType);
    }

    const char* Json::typeAsString(JsonType type)
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


    void Json::deleteChildren()
    {
        if (mType == JsonType::Object){
            delete entries;
        }
        else if (mType == JsonType::Array) {
            delete elements;
        }
        else if (mType == JsonType::String) {
            delete string;
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
        string(strdup(string))
    {
    }

    Json::Json(std::string string) :
        mType(JsonType::String),
        string(strdup(string.data()))
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


    Json& Json::operator[](const char *index)
    {
        typeAssert("operator[const char*]", JsonType::Object);
        return (*entries)[index];
    }

    Json& Json::operator[](char *index)
    {
        return operator[]((const char*) index);
    }

    Json& Json::operator[](std::string index)
    {
        return operator[](index.data());
    }

    Json& Json::operator[](int index)
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


    bool Json::contains(const char* key)
    {
        typeAssert("contains(const char*)", JsonType::Object);
        return entries->find(key) > -1;
    }

    uint Json::size()
    {
        uint length = 0;
        if (mType == JsonType::Array) {
            return elements->size();
        }
        else if (mType == JsonType::Object) {
            return entries->size();
        }
        else {
            throw RuntimeError("size() can only be called on Objects or Arrays");
        }
        return length;
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

    void Json::typeAssert(const char* operation, JsonType type) {
        if (type != mType) {
            throw RuntimeError("Attempting %s on json of type %s", operation, typeAsString());
        }
    }

    Json Json::take(const char* key)
    {
        typeAssert("take(const char*)", JsonType::Object);
        return entries->take(key);
    }


    org_restfulipc::Json::operator bool()
    {
        typeAssert("operator bool()", JsonType::Boolean);
        return boolean;
    }

    org_restfulipc::Json::operator long()
    {
        typeAssert("operator long()", JsonType::Number);
        return (long)number;
    }

    org_restfulipc::Json::operator double()
    {
        typeAssert("operator double()", JsonType::Number);
        return number;
    }

    org_restfulipc::Json::operator const char *()
    {
        typeAssert("operator const char*()", JsonType::String);
        return string;
    }

    Json& operator<<(Json& json, const char* serialized)
    {

        JsonReader reader(serialized);
        json = reader.read();
        return json;
    }

}
