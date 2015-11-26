#include "jsonreader.h"
#include "json.h"

namespace org_restfulipc
{

    const char *Json::typeAsString()
    {
        static const char* typeNames[] = { "NoType0", "Object", "Array", "String", "Long", "Double", "Boolean", "Null" };
        return typeNames[(uint8_t)mType];
    }

    void Json::deleteChildren()
    {
        if (mType == JsonType::Object){
            Entry* ptr;
            while (firstEntry) {
                ptr = firstEntry;
                firstEntry = firstEntry->next;
                delete ptr;
            }
        }
        else if (mType == JsonType::Array) {
            Element* ptr;
            while (firstElement) {
                ptr = firstElement;
                firstElement = firstElement->next;
                delete ptr;
            }
        }
    }

    Json&Json::insertUndefinedAt(uint index)
    {
        typeAssert(JsonType::Array);
        Element** ptr = &firstElement;
        while (index > 0 && *ptr) {
            index--;
            ptr = &((*ptr)->next);
        }
        if (index> 0) {
            throw RuntimeError("Out of range");
        }
        Element* newElement = new Element();
        newElement->next = *ptr;
        *ptr = newElement;
        return *newElement;
    }

    Json&Json::appendUndefinded()
    {
        typeAssert(JsonType::Array);
        Element** ptr = &firstElement;
        while (*ptr) {
            ptr = &((*ptr)->next);
        }
        *ptr = new Element();
        return **ptr;
    }

    Json::~Json()
    {
        deleteChildren();
    }

    Json &Json::operator[](const char *index)
    {
        typeAssert(JsonType::Object);
        Entry** ptr = &firstEntry;
        while (*ptr && strcmp(index, (*ptr)->key)) {
            ptr = &((*ptr)->next);
        }
        if (! (*ptr)) {
            *ptr = new Entry();
            (*ptr)->key = index;
        }

        return **ptr;
    }

    Json &Json::operator[](uint index)
    {
        typeAssert(JsonType::Array);
        Element** ptr = &firstElement;
        while (*ptr && index > 0) {
            ptr = &((*ptr)->next);
            index--;
        }
        if (index >0) {
            throw RuntimeError("Out of range");
        }
        return **ptr;
    }

    Json *Json::at(const char* index)
    {
        typeAssert(JsonType::Object);
        for (Entry* ptr = firstEntry; ptr; ptr = ptr->next) {
            if (! strcmp(index, ptr->key)) {
                return ptr;
            }
        }

        return NULL;
    }

    Json *Json::at(uint index)
    {
        typeAssert(JsonType::Array);
        for (Element* ptr = firstElement; ptr; ptr = ptr->next) {
            if (index == 0) {
                return ptr;
            }
            index--;
        }

        return NULL;
    }

    Json&Json::operator=(JsonType jsonType)
    {
        deleteChildren();
        memset(this, 0, sizeof(Json));
        mType = jsonType;
    }

    Json &Json::operator=(const char *string)
    {
        deleteChildren();
        mType = JsonType::String;
        this->string = string;
    }

    Json &Json::operator=(double d)
    {
        deleteChildren();
        mType = JsonType::Double;
        this->numberD = d;
    }

    Json &Json::operator=(int i)
    {
        deleteChildren();
        mType = JsonType::Long;
        this->numberL = (long)i;
    }

    Json &Json::operator=(long l)
    {
        deleteChildren();
        mType = JsonType::Long;
        this->numberL = l;
    }

    Json &Json::operator=(bool b)
    {
        deleteChildren();
        mType = JsonType::Boolean;
        boolean = b;
    }

    Json*Json::take(int index)
    {
       if (index < 0) {
           throw RuntimeError("Out of range");
       }
       return take((uint) index);
    }

    Json*Json::take(uint index)
    {
        typeAssert(JsonType::Array);
        Element **ptr = &firstElement;
        while (index > 0 && *ptr) {
            index--;
            ptr = &((*ptr)->next);
        }
        if (!(*ptr)) {
            throw RuntimeError("Out of range");
        }
        Element* taken = *ptr;
        *ptr = taken->next;
        taken->next = NULL;
        return taken;
    }

    bool Json::contains(const char* key)
    {
        typeAssert(JsonType::Object);
        for (Entry* entry = firstEntry; entry; entry = entry->next) {
            if (! strcmp(key, entry->key)) {
                return true;
            }
        }

        return false;
    }

    uint Json::size()
    {
        uint length = 0;
        if (mType == JsonType::Array) {
            for (Element* element = firstElement; element; element = element->next) {
                length++;
            }
        }
        else if (mType == JsonType::Object) {
            for (Entry* entry = firstEntry; entry; entry = entry->next) {
                length++;
            }
        }
        else {
            throw RuntimeError("size() can only be called on Objects or Arrays");
        }
        return length;
    }

    void Json::typeAssert(JsonType otherType) {
        if (otherType != mType) {
            throw org_restfulipc::RuntimeError("Type mismatch");
        }
    }

    Json*Json::take(const char* key)
    {
        typeAssert(JsonType::Object);
        for (Entry** ptr = &firstEntry; *ptr; ptr = &((*ptr)->next)) {
            if (! strcmp(key, (*ptr)->key)) {
                Entry* taken = *ptr;
                *ptr = (*ptr)->next;
                taken->next = NULL;
                return taken;
            }
        }

        throw RuntimeError("Not found");
    }

    void Json::remove(uint index)
    {
        delete take(index);
    }

    void Json::remove(const char* key)
    {
        delete take(key);
    }

    org_restfulipc::Json::operator bool()
    {
        typeAssert(JsonType::Boolean);
        return boolean;
    }

    org_restfulipc::Json::operator long()
    {
        typeAssert(JsonType::Long);
        return numberL;
    }

    org_restfulipc::Json::operator double()
    {
        typeAssert(JsonType::Double);
        return numberD;
    }

    org_restfulipc::Json::operator const char *()
    {
        typeAssert(JsonType::String);
        return string;
    }

    Json& operator<<(Json& json, char* serialized)
    {

        JsonReader reader(serialized);
        reader.read(&json);
        return json;
    }


}
