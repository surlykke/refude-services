#include "jsonreader.h"
#include "json.h"

namespace org_restfulipc
{

    const char* Json::typeAsString() {
        return typeAsString(mType);
    }

    const char* Json::typeAsString(JsonType type)
    {
        static const char* typeNames[] = { "Undefined", "Object", "Array", "String", "Long", "Double", "Boolean", "Null" };
        return typeNames[(uint8_t)type];
    }

    void Json::deleteChildren()
    {
        if (mType == JsonType::Object){
            map_delete(entries);
        }
        else if (mType == JsonType::Array) {
            list_delete(elements);
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
            entries = map_create<Json>();
            break;
        case JsonConst::EmptyArray:
            mType = JsonType::Array;
            elements = list_create<Json>();
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

    Json::~Json()
    {
        deleteChildren();
    }

    Json& Json::operator=(Json&& other)
    {
        deleteChildren();
        memcpy(this, &other, sizeof(Json));
        memset(&other, 0, sizeof(Json));
    }


    Json& Json::operator[](const char *index)
    {
        typeAssert(JsonType::Object);
        return map_at(entries, index);
    }

    Json& Json::operator[](int index)
    {
        typeAssert(JsonType::Array);
        return list_at(elements, index);
    }

    Json&Json::operator=(JsonConst jsonConst)
    {
        deleteChildren();
        switch (jsonConst) {
        case JsonConst::EmptyObject:
            mType = JsonType::Object;
            entries = map_create<Json>();
            break;
        case JsonConst::EmptyArray:
            mType = JsonType::Array;
            elements = list_create<Json>();
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

    Json& Json::operator=(const char *string)
    {
        deleteChildren();
        mType = JsonType::String;
        this->string = string;
    }

    Json &Json::operator=(double number)
    {
        deleteChildren();
        mType = JsonType::Number;
        this->number = number;
    }


    Json&& Json::take(int index)
    {
        typeAssert(JsonType::Array);
        return list_take(elements, index);
    }


    bool Json::contains(const char* key)
    {
        typeAssert(JsonType::Object);
        return map_contains(entries, key);
    }

    uint Json::size()
    {
        uint length = 0;
        if (mType == JsonType::Array) {
            return elements->size;
        }
        else if (mType == JsonType::Object) {
            return entries->size;
        }
        else {
            throw RuntimeError("size() can only be called on Objects or Arrays");
        }
        return length;
    }

    void Json::append(Json&& json)
    {
        typeAssert(JsonType::Array);
        list_append(elements, std::move(json));
    }

    void Json::insertAt(int index, Json&& json)
    {
        typeAssert(JsonType::Array);
        list_insert(elements, std::move(json), index);
    }

    void Json::typeAssert(JsonType otherType) {
        if (otherType != mType) {
            throw org_restfulipc::RuntimeError(std::string("Type mismatch - expected ") +
                                               typeAsString() + " got " + typeAsString(otherType));
        }
    }

    Json&& Json::take(const char* key)
    {
        typeAssert(JsonType::Object);
        return map_take(entries, key);
    }


    org_restfulipc::Json::operator bool()
    {
        typeAssert(JsonType::Boolean);
        return boolean;
    }

    org_restfulipc::Json::operator long()
    {
        typeAssert(JsonType::Number);
        return (long)number;
    }

    org_restfulipc::Json::operator double()
    {
        typeAssert(JsonType::Number);
        return number;
    }

    org_restfulipc::Json::operator const char *()
    {
        typeAssert(JsonType::String);
        return string;
    }

    Json& operator<<(Json& json, char* serialized)
    {

        JsonReader reader(serialized);
        json = reader.read();
        return json;
    }


}
