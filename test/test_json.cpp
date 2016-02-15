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
    std::cout << "Start building\n";
    Json json = JsonConst::EmptyObject;
    for (int i = 0; i < 5000000; i++) {
        json.append("foo", 1);
        json.append("baa", 2);
        json.append("yxy", 3);
    }
    std::cout << "Start sorting\n";
    std::cout << JsonWriter(&json).buffer.data << "\n";
}
