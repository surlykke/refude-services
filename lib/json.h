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

    class Json
    {
    public:
        Json() : string(0), mType(JsonType::Undefined) {}
        ~Json();

        Json& operator[](const char* index);
        Json& operator[](uint index);

        Json* at(const char *index);
        Json* at(uint index);

        Json& operator=(JsonType jsonType);
        Json& operator=(const char* string);
        Json& operator=(double d);
        Json& operator=(int i);
        Json& operator=(long l);
        Json& operator=(bool b);

        operator const char*();
        operator double();
        operator long();
        operator bool();

        Json* take(int index);
        Json* take(uint index);
        void remove(uint index);
        Json* take(const char* key);
        void remove(const char* key);
        bool contains(const char* key);
        uint size();

        template<typename T> Json& append(T t)
        {
            Json& appended = appendUndefinded();
            return appended = t;
        }

        template<typename T> Json& insertAt(uint index, T t)
        {
            Json& inserted = insertUndefinedAt(index);
            return inserted = t;
        }

        void typeAssert(JsonType otherType);
        const char* typeAsString();

    private:
        void deleteChildren();
        Json& insertUndefinedAt(uint index);
        Json& appendUndefinded();

        union {
            Element* firstElement;
            Entry* firstEntry;
            const char* string;
            double numberD;
            long numberL;
            bool boolean;
        };
        JsonType mType;

        friend class JsonReader;
        friend class JsonWriter;
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
