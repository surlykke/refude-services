#ifndef JSON_H
#define JSON_H
#include <memory>
#include <string>
#include <stdint.h>
#include "errorhandling.h"
#include "map.h"

namespace org_restfulipc
{
    enum JsonType
    {
        Undefined = 0, // Important that this is 0 - so when we zero out a json, it will have type Undefined)
        Object,
        Array,
        String,
        Number,
        Boolean,
        Null
    };

    // Used for initializing
    enum class JsonConst
    {
        EmptyObject,
        EmptyArray,
        TRUE,
        FALSE,
        Null

    };


    class Json
    {
    public:
        Json() : mType(JsonType::Undefined) {}
        Json(Json& other) = delete;
        Json(Json&& other);
        Json(JsonConst jsonConst);
        Json(const char* string);
        Json(std::string string);
        Json(double number);
        ~Json();

        Json deepCopy();
        Json& operator=(Json&& other);
        Json& operator=(Json& other) = delete;
        Json& operator=(JsonConst jsonConst);
        bool operator==(const Json& other) const;
        bool operator!=(const Json& other) const;

        Json& operator[](const char* index);
        Json& operator[](std::string index);
        Json& operator[](int index) const;

        operator const char*() const;
        operator double() const;
        operator long() const;
        operator bool() const;

        Json take(int index);
        Json take(const char* key);
        bool contains(const char* key) const;
        const char* keyAt(size_t index) const;
        uint size() const;

        Json& append(Json&& json);
        Json& append(const char* key, Json&& json);
        Json& insertAt(int index, Json&& json);

        void typeAssert(const char* operation, JsonType otherType) const;
        const char* typeAsString() const;
        const char* typeAsString(const JsonType type) const;
        bool undefined();
        JsonType type();
    private:
        void deleteChildren();

        union {
            Map<Json>*  entries;
            std::vector<Json>* elements;
            const char* str;
            double number;
            bool boolean;
        };
        JsonType mType;

        friend class JsonReader;
        friend class JsonWriter;
        friend class FilteringJsonWriter;
    };

    Json& operator<<(Json& json, const char* serialized);
}
#endif // JSON_H
