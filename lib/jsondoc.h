/* 
 * File:   json.h
 * Author: christian
 *
 * Created on 23. juli 2015, 10:32
 */

#ifndef JSONDOC_H
#define JSONDOC_H

#include <string.h>
#include <stdint.h>

#include <iostream>
#include <memory>
#include <cstddef>
#include <algorithm>
#include <climits>

#include "errorhandling.h"
#include "json.h"

namespace org_restfulipc
{

    class JsonDoc
    {
    public:
        JsonDoc();
        JsonDoc(char* buf);
        virtual ~JsonDoc() {}

        JsonReference operator[](const char* index);
        JsonReference operator[](uint32_t index);

        JsonReference root();

        void write();

    private:
        JsonStruct* jsonStruct;
    };



}

#endif /* JSONDOC_H */

