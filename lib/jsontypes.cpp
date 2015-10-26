#include <stdio.h>
#include <string.h>
#include "jsontypes.h"
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

    int JsonString::serializedLength()
    {
        if (!string) { // NULL
            return 4;
        }
        else {
            int length = 0;

            for (const char* c = string; *c; c++) {
                // We escape '\'s and '"'s
                length += (*c == '\\' || *c == '"') ? 2 : 1;
            }

            return length + 2; // 2 enclosing '"'s
        }
    }

    void JsonString::serialize(char *&dest)
    {
        if (!string) {
            dest += sprintf(dest, "NULL");
        }
        else {
            *(dest++) = '"';
            for (const char* c = string; *c; c++) {
                if (*c == '\\' || *c == '"') {
                    *(dest++) = '\\';
                }
                *(dest++) = *c;
            }
            *(dest++) = '"';
            *dest = '\0';
        }
    }

    void JsonString::serialize()
    {
        std::cout << "\"" << string << "\""; // FIX escapes
    }

    int JsonNumber::serializedLength()
    {
        return strlen(number);
    }

    void JsonNumber::serialize(char *&dest)
    {
        dest += sprintf(dest, "%s", number);
    }

    void JsonNumber::serialize()
    {
        std::cout << number;
    }

    int JsonBoolean::serializedLength()
    {
        return boolean ? 4 : 5;
    }

    void JsonBoolean::serialize(char *&dest)
    {
        strcpy(dest, boolean ? "true" : "false") ;
        dest += boolean ? 4 : 5;
        *dest = '\0';
    }

    void JsonBoolean::serialize()
    {
        std::cout << boolean;
    }

    AbstractJson *JsonArray::element(unsigned int index)
    {
        JsonArrayEntry* ptr = first;
        while (ptr && index > 0) {
            ptr = ptr->next;
            index--;
        }

        if (!ptr) throw RuntimeError("Out of bounds");

        return ptr->element;
    }

    void JsonArray::insertAt(unsigned int at, AbstractJson *element, Heap *heap)
    {
        // FIXME
    }

    void JsonArray::append(AbstractJson *element, Heap *heap)
    {
        JsonArrayEntry* entry = new (heap->allocate<JsonArrayEntry>()) JsonArrayEntry();
        entry->element = element;

        if (last) {
            last->next = entry;
        }
        else {
            first = entry;
        }

        last = entry;
    }

    void JsonArray::remove(unsigned int, Heap *heap)
    {

    }

    int JsonArray::serializedLength()
    {
        int accum = 0;
        for (JsonArrayEntry* ptr = first; ptr; ptr = ptr->next) {
            accum += ptr->element->serializedLength();
            if (ptr != first) accum += 2;
        }

        return accum + 2;
    }

    void JsonArray::serialize(char *&dest)
    {
        *(dest++) = '[';
        for (JsonArrayEntry* ptr = first; ptr; ptr = ptr->next) {
            if (ptr != first) {
                strcpy(dest, ", ");
                dest += 2;
            }
            ptr->element->serialize(dest);
        }
        *(dest++) = ']';
        *dest = '\0';
    }

    void JsonArray::serialize()
    {
        std::cout << "[" ;
        const char* sep = "";
        for (JsonArrayEntry* ptr = first; ptr; ptr = ptr->next) {
            std::cout << sep;
            ptr->element->serialize();
            sep = ", ";
        }
        std::cout << "]";

    }

    AbstractJson *JsonObject::value(const char *key)
    {
        for (JsonObjectEntry* ptr = first; ptr; ptr = ptr->next)
            if (!strcmp(key, ptr->key))
                return ptr->value;

        return 0;
    }

    void JsonObject::put(const char *key, AbstractJson *value, Heap *heap)
    {
        JsonObjectEntry* newEntry = new (heap->allocate<JsonObjectEntry>()) JsonObjectEntry();
        newEntry->key = key;
        newEntry->value = value;

        if (last) {
            last->next = newEntry;
        }
        else {
            first = newEntry;
        }

        last = newEntry;
    }

    void JsonObject::remove(const char *key, Heap *heap)
    {
       // FIXME
    }

    int JsonObject::serializedLength() {
        int accum = 0;
        for (JsonObjectEntry* ptr = first; ptr; ptr = ptr->next) {
            accum += strlen(ptr->key) + 5;
            accum += ptr->value->serializedLength();
            if (ptr != first) {
                accum += 2; // comma + space
            }
        }

        return accum + 2;
    }

    void JsonObject::serialize(char *&dest)
    {
        *(dest++) = '{';

        const char* separator = "";
        for (JsonObjectEntry* ptr = first; ptr; ptr = ptr->next) {
            dest += sprintf(dest, "%s\"%s\" : ", separator, ptr->key);
            ptr->value->serialize(dest);
            separator = ", ";
        }

        *(dest++) = '}';
        *dest = '\0';
    }

    void JsonObject::serialize()
    {
        std::cout << "{";
        const char* sep = "";
        for (JsonObjectEntry* ptr = first; ptr; ptr = ptr->next) {
            std::cout << sep << "\"" << ptr->key << "\" : ";
            ptr->value->serialize();
            sep = ", ";
        }
        std::cout << "}";
    }




}
