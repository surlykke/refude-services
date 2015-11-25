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

    Json::~Json()
    {
        deleteChildren();
    }

    Json &Json::operator[](const char *index)
    {
        typeAssert(JsonType::Object);
        Entry*& ptr = firstEntry;
        while (ptr && strcmp(index, ptr->key)) {
            ptr = ptr->next;
        }
        if (! ptr) {
            ptr = new Entry();
            ptr->key = index;
        }
        return *ptr;
    }

    Json &Json::operator[](uint index)
    {
        typeAssert(JsonType::Array);
        Element*& ptr = firstElement;
        while (ptr && index > 0) {
            ptr = ptr->next;
            index--;
        }
        if (index >0) {
            throw RuntimeError("Out of range");
        }
        return *ptr;
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
