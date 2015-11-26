#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jsonwriter.h"
#include "json.h"

using namespace org_restfulipc;

int main()
{

    Json json;
    json = JsonType::Object;
    json["foo"] = 7;
    json["baa"] = false;
    json.take("foo");
    std::cout << json.size() << "\n";
    JsonWriter(&json).write();
    std::cout << "\n";

    json = JsonType::Array;
    json.append( 1);
    json.append( 2);
    json.insertAt(1, true);
    JsonWriter(&json).write();
    std::cout << "\n";
    json.remove(2);
    Json* taken = json.take(0);
    long l = *taken;
    std::cout << "At 0: " << l << "\n";
    delete taken;
    std::cout << "taken deleted\n";
    JsonWriter(&(json)).write();
    std::cout << "\n";
    json.insertAt(17, JsonType::Null);
}
