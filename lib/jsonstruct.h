#ifndef JSONSTRUCT_H
#define JSONSTRUCT_H

#include <stdint.h>
#include <string>

#include "errorhandling.h"

namespace trash
{
    enum class JsonType : uint32_t
    {
        Undefined = 0,
        Object    = (uint32_t) 1 << 29,
        Array     = (uint32_t) 2 << 29,
        String    = (uint32_t) 3 << 29,
        Boolean   = (uint32_t) 4 << 29,
        Long      = (uint32_t) 5 << 29,
        Double    = (uint32_t) 6 << 29,
        Null      = (uint32_t) 7 << 29
    };

    /**
     * We use 32 bit words as json pointers
     * The topmost 3 bits are used as type-tags,
     * leaving 29 bytes for the value part
     */
    class Json
    {
    public:
        Json(JsonType type, uint32_t val);

        static const Json EmptyObject;
        static const Json EmptyArray;
        static const Json False;
        static const Json True;
        static const Json Null;
        static const Json Undefined;

        JsonType type() { return (JsonType) (_d & 0xE0000000); }
        uint32_t value() { return (_d & 0x1FFFFFFF); }
        bool isType(JsonType);
        void typeAssert(JsonType tag);
        std::string toString();

        friend bool operator==(Json& one, Json& other);
        friend bool operator!=(Json& one, Json& other);

    private:
        uint32_t _d;
    };

    bool operator==(Json& one, Json& other);
    bool operator!=(Json& one, Json& other);

    template<typename ElementType> JsonType jsonTypeTag();

    struct ObjectEntry
    {
        Json key;
        Json value;
        Json next;
    };

    struct ArrayEntry
    {
        Json value;
        Json next;
    };


    template<typename ElementType>
    struct Pool
    {
        Pool(uint32_t initialsize, uint32_t& availableBytes):
            data(0),
            used(1),
            size(initialsize),
            availableBytes(availableBytes)
        {
            if (size = 0) {
                size = 1;
            }

            if (size*sizeof(ElementType) > availableBytes) {
                throw org_restfulipc::RuntimeError("Initial size execes available memory");
            }

            data = (ElementType*) calloc(size, sizeof(ElementType));
            availableBytes -= size*sizeof(ElementType);

            if (!data) {
                throw org_restfulipc::RuntimeError("Initial allocation failed");
            }
        }


        ~Pool()
        {
            free(data);
        }

        void setData(ElementType* data, uint32_t used, uint32_t size)
        {
            free(this->data);
            availableBytes += this->size*sizeof(ElementType);

            this->data = data;
            this->used = used;
            this->size = size;
            availableBytes -= this->size*sizeof(ElementType);
        }

        Json allocate(uint32_t required = 1)
        {
            if (used + required > size) {
                uint32_t elementsToAllocate = size; // By default we double data size
                uint32_t numberOfElementsThatCanBeAllocated = availableBytes/sizeof(ElementType);
                if (elementsToAllocate > numberOfElementsThatCanBeAllocated) {
                    elementsToAllocate = numberOfElementsThatCanBeAllocated;
                    if (used + required > size + elementsToAllocate) {
                        throw org_restfulipc::RuntimeError("Unable to allocate sufficient");
                    }
                }

                data = (ElementType*) realloc(data, (size + elementsToAllocate)*sizeof(ElementType));
                availableBytes -= elementsToAllocate*sizeof(ElementType);

                if (! data)  {
                    throw org_restfulipc::RuntimeError("Reallocation failed");
                }

            }

            used += required;
            return Json(jsonTypeTag<ElementType>(), used - required);
        }

        ElementType* data;
        uint32_t used;
        uint32_t size;
        uint32_t& availableBytes;

        ElementType& operator[](Json json)
        {
            json.typeAssert(jsonTypeTag<ElementType>());
            return data[json.value()];
        }

        ElementType* at(Json json)
        {
            json.typeAssert(jsonTypeTag<ElementType>());
            return data + json.value();
        }
    };


    struct JsonStruct
    {

        JsonStruct(uint32_t maxMemoryUse) :
            unusedMemory(maxMemoryUse),
            charPool(256, unusedMemory),
            longPool(64, unusedMemory),
            doublePool(64, unusedMemory),
            arrayEntryPool(64, unusedMemory),
            objectEntryPool(64, unusedMemory),
            root(Json::Undefined)
        {
        }

        uint32_t unusedMemory;
        Pool<char> charPool;
        Pool<long> longPool;
        Pool<double> doublePool;
        Pool<ArrayEntry> arrayEntryPool;
        Pool<ObjectEntry> objectEntryPool;

        Json root;

    };


}



#endif // JSONSTRUCT_H
