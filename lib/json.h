#ifndef JSON_H
#define JSON_H
#include <memory>
#include <stdint.h>
#include "errorhandling.h"
#include "map.h"

namespace org_restfulipc
{

    enum JsonType
    {
        Undefined = 0, // Important that this i 0. (So when we zero out a json, it will have type Undefined)
        Object,
        Array,
        String,
        Number,
        Boolean,
        Null
    };

    // Used for initilizing
    enum class JsonConst
    {
        EmptyObject,
        EmptyArray,
        TRUE,
        FALSE,
        Null

    };

    class Element;
    class Entry;

    class Json
    {
    public:
        Json() : mType(JsonType::Undefined) {}
        Json(Json& other) = delete;
        Json(Json&& other);
        Json(JsonConst jsonConst);
        Json(const char* string): mType(JsonType::String), string(string) {}
        Json(double number) : mType(JsonType::Number), number(number) {}
        ~Json();

        Json& operator=(Json&& other);
        Json& operator=(Json& other) = delete;
        Json& operator=(JsonConst jsonConst);
        Json& operator=(const char* string);
        Json& operator=(double number);

        Json& operator[](const char* index);
        Json& operator[](int index);

        operator const char*();
        operator double();
        operator long();
        operator bool();

        Json&& take(int index);
        Json&& take(const char* key);
        bool contains(const char* key);
        uint size();

        Json& append(Json&& json);
        Json& append(const char* key, Json&& json);
        Json& insertAt(int index, Json&& json);

        void typeAssert(const char* operation, JsonType otherType);
        const char* typeAsString();
        const char* typeAsString(JsonType type);

    //private:
        void deleteChildren();

        union {
            Map<Json>*  entries;
            std::vector<Json>* elements;
            const char* string;
            double number;
            bool boolean;
        };
        JsonType mType;

        friend class JsonReader;
        friend class JsonWriter;
    };

    Json& operator<<(Json& json, char* serialized);
}
#endif // JSON_H
