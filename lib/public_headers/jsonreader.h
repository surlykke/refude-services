/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LGPL21 file for a copy of the license.
*/

#ifndef JSONREADER_H
#define JSONREADER_H

#include <stdint.h>
#include "errorhandling.h"
#include "json.h"

namespace refude
{
    class JsonReader
    {
    public:
        JsonReader(const char* buf);
        Json read();

    private:
        void readNext(Json& json);
        const char *readString();
        void replaceEscapes(char* string);
        void readUnicodeEscape(char*& dest, char*& src);
        double readNumber();

        bool currentCharIsWhiteSpace();

        void skip();
        void skip(char c);
        void skipSpace();
        void skipKeyword(const char* string);
        uint16_t hexValue(char c);

        const char* buf;
        const char* cc;
        
        // Error reporting
        int line();
        int column();
    };

}
#endif // JSONREADER_H
