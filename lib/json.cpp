#include "json.h"
namespace org_restfulipc
{



    const Json Json::Undefined = Json(JsonType::NoType);
    const Json Json::True = Json(JsonType::Boolean, true);
    const Json Json::False = Json(JsonType::Boolean, false);
    const Json Json::Null = Json(JsonType::Null);
    const Json Json::emptyObject() { return Json(JsonType::Object); }
    const Json Json::emptyArray() { return Json(JsonType::Array); }

    JsonReference JsonReference::operator[](const char *index)
    {
        if (!strcmp("-", index) == 0 && json.type() == JsonType::Array) {
            Element*& ptr = json.firstElement;
            while (ptr) {
                ptr = ptr->next;
            }
            ptr = new Element(Json::Undefined);
            return JsonReference(jsonStruct, *ptr);
        }
        else {
            json.typeAssert(JsonType::Object);
            Entry*& ptr = json.firstEntry;
            while (ptr && strcmp(ptr->key, index)) {
                ptr = ptr->next;
            }
            if (!ptr) {
                ptr = new Entry(index, Json::Undefined);
            }

            return JsonReference(jsonStruct, *ptr);
        }

    }

    JsonReference JsonReference::operator[](uint32_t index)
    {
        json.typeAssert(JsonType::Array);
        Element* ptr = json.firstElement;
        while (ptr && index>0) {
            ptr = ptr->next;
        }
        if (!ptr) {
            throw RuntimeError("Out of range");
        }
        return JsonReference(jsonStruct, *ptr);
    }

}
