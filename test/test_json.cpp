#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jsonwriter.h"
#include "json.h"

using namespace org_restfulipc;

int main()
{

/*    Json json = JsonConst::EmptyArray;
    for (int i = 0; i < 7; i++) {
        json.append(i);
    }

    json[2] = 17;
    json.insertAt(2, 18);

    JsonWriter(&json).write();*/
    Json json = JsonConst::EmptyObject;

    json["foo"] = JsonConst::Null;
    json["baa"] = "hejsa";
    json["moo"] = 832;
    json["foo"] = JsonConst::TRUE;
    std::cout << JsonWriter(&json).buffer.data << "\n";

}
