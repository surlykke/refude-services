/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef JSON_H
#define JSON_H
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>
#include "map.h"
#include "errorhandling.h"

namespace refude
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
        Json(Json&& other);
        Json(JsonConst jsonConst);
        Json(const char* string);
        Json(std::string string);
        Json(double number);
        ~Json();

        Json copy() const;
        Json& operator=(Json&& other);
        Json& operator=(JsonConst jsonConst);
        bool operator==(const Json& other) const;
        bool operator!=(const Json& other) const;

        Json& operator[](const char* index);
        Json& operator[](std::string index);
        Json& operator[](int index) const;
        std::vector<const char*> keys() const;
        const char* toString() const;
        double toDouble() const;
        bool toBool() const;

        Json take(int index);
        Json take(const char* key);
        void erase(const char* key);
        bool contains(const char* key) const;
        bool contains(std::string key) const;
        int find(const char* str) const;
        int find(std::string str) const;
        const std::string& keyAt(size_t index) const;
        uint size() const;

        Json& append(Json&& json);
        Json& insertAt(int index, Json&& json);
        void append(std::vector<Map<Json>::Entry>&& pairs);

        const char* typeAsString() const;
        const char* typeAsString(const JsonType type) const;
        bool undefined() const;
        JsonType type() const;
        
        template<typename Visitor>
        void eachEntry(Visitor entryVisitor)
        {
            typeAssert(JsonType::Object, "eachEntry(<lambda>)");
            for (auto& entry : *entries) {
                entryVisitor(entry.key, entry.value);
            }
        }

        template<typename Visitor>
        void eachElement(Visitor elementVisitor)
        {
            typeAssert(JsonType::Array, "eachElement(<lambda>)");
            for (int i = 0; i < elements->size(); i++) {
                elementVisitor((*elements)[i]);
            }
        }

    private:
        void typeAssert(JsonType otherType, const char* operationDescFmt, ...) const;
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
        friend class LocalizingJsonWriter;
    };

    Json& operator<<(Json& json, const char* serialized);
}
#endif // JSON_H
