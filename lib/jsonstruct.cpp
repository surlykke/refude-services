#include "errorhandling.h"
#include "jsonstruct.h"

namespace trash
{
    const Json Json::EmptyObject = Json(JsonType::Object, 0);
    const Json Json::EmptyArray = Json(JsonType::Array, 0);
    const Json Json::False = Json(JsonType::Boolean, 0);
    const Json Json::True = Json(JsonType::Boolean, 1);
    const Json Json::Null = Json(JsonType::Null, 0);
    const Json Json::Undefined = Json(JsonType::Undefined, 0);

    Json::Json(JsonType type, uint32_t val)
    {
        _d = (uint32_t)type | val;
    }

    void Json::typeAssert(JsonType tag)
    {
        if (tag != type()) {
            throw org_restfulipc::RuntimeError("Type mismatch");
        }
    }

    std::string Json::toString()
    {
        static std::string typeNames[8] =  { "Unknown", "Object", "Array", "String", "Boolean", "Long", "Double", "Null" };
        return typeNames[uint(type()) >> 29] + "(" + std::to_string(value()) + ")";
    }

    template<> JsonType jsonTypeTag<char>() { return JsonType::String; }
    template<> JsonType jsonTypeTag<long>() { return JsonType::Long; }
    template<> JsonType jsonTypeTag<double>() { return JsonType::Double; }
    template<> JsonType jsonTypeTag<ArrayEntry>() { return JsonType::Array; }
    template<> JsonType jsonTypeTag<ObjectEntry>() { return JsonType::Object; }

    bool operator==(Json &one, Json &other)
    {
        return one._d = other._d;
    }

}
