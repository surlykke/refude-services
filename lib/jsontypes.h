#ifndef JSONTYPES_H
#define JSONTYPES_H
#include <memory>
#include "errorhandling.h"

namespace org_restfulipc
{
    class Heap;

    enum class JsonTypeTag
    {
        Number,
        Boolean,
        String,
        Object,
        Array
    };

    const char* tag2str(JsonTypeTag tag);

    struct AbstractJson
    {
        AbstractJson(JsonTypeTag typeTag) : typeTag(typeTag) {}
        JsonTypeTag typeTag;

        virtual int serializedLength() = 0;
        virtual void serialize(char *&dest) = 0;
        virtual void serialize() = 0;
    };

    struct JsonArrayEntry
    {
        JsonArrayEntry* next;
        AbstractJson* element;
    };

    struct JsonObjectEntry
    {
        JsonObjectEntry* next;
        const char* key;
        AbstractJson* value;
    };


    struct JsonString : AbstractJson
    {
        JsonString(const char* string) : AbstractJson(JsonTypeTag::String), string(string) {}
        virtual int serializedLength();
        virtual void serialize(char *&dest);
        virtual void serialize();

        const char* string;
    };


    struct JsonNumber : AbstractJson
    {
        JsonNumber(const char* number) : AbstractJson(JsonTypeTag::Number), number(number) {}
        virtual int serializedLength();
        virtual void serialize(char *&dest);
        virtual void serialize();

        const char* number;
    };

    struct JsonBoolean : AbstractJson
    {
        JsonBoolean(bool boolean) : AbstractJson(JsonTypeTag::Boolean), boolean(boolean) {}
        virtual int serializedLength();
        virtual void serialize(char *&dest);
        virtual void serialize();

        bool boolean;
    };

    struct JsonArray : AbstractJson
    {
        JsonArray() : AbstractJson(JsonTypeTag::Array), first(0), last(0) {}
        AbstractJson* element(unsigned int index);
        void insertAt(unsigned int at, AbstractJson* element, Heap* heap);
        void append(AbstractJson* element, Heap* heap);
        void remove(unsigned int, Heap* heap);

        virtual int serializedLength();
        virtual void serialize(char *&dest);
        virtual void serialize();

        JsonArrayEntry* first;
        JsonArrayEntry* last;
    };

    struct JsonObject : AbstractJson
    {
        JsonObject(): AbstractJson(JsonTypeTag::Object), first(0), last(0) {}
        AbstractJson* value(const char* key);
        void put(const char* key, AbstractJson* value, Heap* heap);
        void remove(const char* key, Heap* heap);

        virtual int serializedLength();
        virtual void serialize(char *&dest);
        virtual void serialize();

        JsonObjectEntry* first;
        JsonObjectEntry* last;

    };

    #define CHUNK_SIZE 8192

    class Heap
    {
    public:
        Heap() : buffer((char*) malloc(CHUNK_SIZE)), p(buffer), bufferSize(CHUNK_SIZE), freeSize(CHUNK_SIZE) {}
        virtual ~Heap() { free(buffer); }

        template<typename T>
        void* allocate()
        {
            if (! std::align(alignof(T), sizeof(T), p, freeSize)) {
                bufferSize += CHUNK_SIZE;
                freeSize += CHUNK_SIZE;
                buffer = realloc(buffer, bufferSize);
                if (! std::align(alignof(T), sizeof(T), p, freeSize)) {
                    throw RuntimeError("Allocation failed");
                }
            }

            void *result = p;
            p = (char*)p + sizeof(T);
            freeSize -= sizeof(T);
            return result;
        }


    private:
        void* buffer;
        void* p;
        size_t bufferSize;
        size_t freeSize;

    };
}


#endif // JSONTYPES_H
