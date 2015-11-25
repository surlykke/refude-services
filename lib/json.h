#ifndef JSON_H
#define JSON_H
#include <memory>
#include <stdint.h>
#include "errorhandling.h"

namespace org_restfulipc
{

    enum class JsonType : uint8_t
    {
        Undefined = 0,
        Object,
        Array,
        String,
        Long,
        Double,
        Boolean,
        Null
    };

    class Element;
    class Entry;

    struct Json
    {
        Json() : string(0), mType(JsonType::Undefined) {}
        ~Json();

        Json& operator[](const char* index);
        Json& operator[](uint index);

        Json* at(const char *index);
        Json* at(uint index);

        Json& operator=(const char* string);
        Json& operator=(double d);
        Json& operator=(long l);
        Json& operator=(bool b);

        operator const char*();
        operator double();
        operator long();
        operator bool();

        void typeAssert(JsonType otherType) { if (otherType != mType) throw org_restfulipc::RuntimeError("Type mismatch"); }
        template<typename T> T value();
        const char* typeAsString();

        void deleteChildren();

        JsonType mType;
        union {
            Element* firstElement;
            Entry* firstEntry;
            const char* string;
            double numberD;
            long numberL;
            bool boolean;
        };

    };

    Json& operator<<(Json& json, char* serialized);

    struct Element : public Json
    {
        Element() : Json(), next(0) {}
        Element* next;
    };

    struct Entry : public Json
    {
        Entry() : Json(), key(0), next(0) {}
        const char* key;
        Entry* next;
    };


}
#endif // JSON_H
