/*
 * File:   json.cpp
 * Author: christian
 * 
 * Created on 23. juli 2015, 10:32
 */

#include <stdlib.h>
#include <time.h>
#include <iomanip>

#include "errorhandling.h"
#include "jsonstruct.h"
#include "jsonreader.h"
#include "jsonwriter.h"
#include "jsondoc.h"

namespace org_restfulipc
{

    JsonDoc::JsonDoc() :
        jsonStruct(new JsonStruct())
    {
    }

    JsonDoc::JsonDoc(char *buf) : jsonStruct(0)
    {
        JsonReader reader(buf);
        jsonStruct = reader.jsonStruct;
    }

    JsonReference JsonDoc::operator[](const char *index)
    {
        return root()[index];
    }

    JsonReference JsonDoc::operator[](uint32_t index)
    {
        return root()[index];
    }

    JsonReference JsonDoc::root()
    {
        return JsonReference(jsonStruct, jsonStruct->root);
    }

    void JsonDoc::write()
    {
        JsonWriter(jsonStruct).write();
    }

}


