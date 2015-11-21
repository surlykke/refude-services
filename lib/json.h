#ifndef JSON_H
#define JSON_H
#include "errorhandling.h"

namespace org_restfulipc
{

    enum class JsonType
    {
        NoType,
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
    friend class JsonReference;
    friend class JsonWriter;
    friend class JsonReader;

    public:
        static const Json Undefined;
        static const Json True;
        static const Json False;
        static const Json Null;
        static const Json emptyObject();
        static const Json emptyArray();

        Json(char* string) : string(string), mType(JsonType::String) {}
        Json(double d) : numberD(d), mType(JsonType::Double) {}
        Json(long l) : numberL(l), mType(JsonType::Long) {}

        void typeAssert(JsonType otherType) { if (otherType != mType) throw org_restfulipc::RuntimeError("Type mismatch"); }
        JsonType type() { return mType; }

        template<typename T> T value();
        Json& operator=(const Json& other) { mType = other.mType; numberL = other.numberL; return *this; }
        Json& operator[](const char* index);
        Json& operator[](uint64_t index);

    private:

        union {
            Element* firstElement;
            Entry* firstEntry;
            const char* string;
            double numberD;
            long numberL;
            bool boolean;
        };
        JsonType mType;

        Json(JsonType type, bool b = false) : mType(type), firstElement(0) {}
    };

    struct Element : public Json
    {
        Element(Json value) : Json(value), next(0) {}
        Element* next;
    };

    struct Entry : public Json
    {
        Entry(const char* key, Json value) : Json(value), key(key), next(0) {}
        const char* key;
        Entry* next;
    };

    class JsonStruct
    {
    public:
        JsonStruct(int initialHeapSize = 1024, int maxHeapSize = 1048576 /* 1M */) :
            root(Json::Undefined),
            maxHeapSize(maxHeapSize),
            heapSize(heapSize),
            heap(0)
        {
            if (maxHeapSize < heapSize) {
                throw RuntimeError("Max heapsize smaller than initialHeapSize");
            }
            heap = new char[heapSize];
        }

        Json root;

    private:
        template<typename T> T* allocate()
        {

        }

        int maxHeapSize;
        int heapSize;
        int heapUsed;
        char* heap;
    };


    class JsonReference
    {
    public:
        JsonReference operator[](const char* index);
        JsonReference operator[](uint32_t index);

        template<typename T> JsonReference& operator=(T& val)
        {
            json = Json(val);
            return *this;
        }

        JsonReference(JsonStruct *jsonStruct, Json& json) :
            jsonStruct(jsonStruct),
            json(json)
        {
        }



        JsonStruct* jsonStruct;
        Json& json;

    };




}
#endif // JSON_H
