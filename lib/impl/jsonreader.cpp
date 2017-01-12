/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <sstream>
#include <math.h>
#include <bitset>
#include "jsonreader.h"

namespace refude
{
    bool* initializeSpaceTable() {
        bool* table = new bool[256] { false };
        table[0x09] = table[0x0A] = table[0x0D] = table[0x20] = true;
        return table;
    }

    uint16_t* initializeHexTable() {
        uint16_t* table = new uint16_t[256] { 16 /* used as invalid marker*/ };
        for (int i = '0'; i <= '9'; i++) {
            table[i] = i - '0';
        }
        for (int i = 'a'; i <= 'f'; i++) {
            table[i] = table[i + 'A' - 'a'] = 10 + i - 'a';
        }
        return table;
    }

    char* initializeEscapeTable() {
        char* table = new char[256] {'\0'};
        table['/'] = '/';
        table['\\'] = '\\';
        table['"'] = '"';
        table['t'] = '\t';
        table['n'] = '\n';
        table['b'] = '\b';
        table['r'] = '\r';
        table['f'] = '\f';
        return table;
    }

    JsonReader::JsonReader(const char *buf) :
        buf(buf), 
        cc(buf)
    {
    }

    Json JsonReader::read()
    {
        skipSpace();
        Json json;
        readNext(json);

        if (*cc != '\0') {
            throw RuntimeError("Trailing character: %c at %d,%d", *cc, line(), column());
        }
        
        return Json(std::move(json));
    }

    void JsonReader::readNext(Json& json)
    {
        if (*cc == '{') {
            json = JsonConst::EmptyObject;
            skip();
            if (*cc != '}') {
                std::vector<Map<Json>::Entry> collectedElements;
                for(;;) {
                    const char* entryKey = readString();
                    skip(':');
                    Map<Json>::Entry pair(entryKey, Json());
                    readNext(pair.value);
                    collectedElements.push_back(std::move(pair));
                    if (*cc != ',') break;
                    skip();
                    if (*cc == '}') break;
                }
                json.append(std::move(collectedElements));
            }
            skip('}');
        }
        else if (*cc == '[')  {
            json = JsonConst::EmptyArray;
            skip();
            if (*cc != ']') {
                for (;;) {
                    Json& element = json.append(Json());
                    readNext(element);
                    if (*cc != ',') break;
                    skip();
                    if (*cc == ']') break;
                }
            }
            skip(']');
        }
        else if (*cc == '"') {
            json = readString();
        }
        else if (*cc == 't') {
            skipKeyword("true");
            json = JsonConst::TRUE;
        }
        else if (*cc == 'f') {
            skipKeyword("false");
            json = JsonConst::FALSE;
        }
        else if (*cc == 'n') {
            skipKeyword("null");
            json = JsonConst::Null;
        }
        else if (*cc == '-' || (*cc >= '0' && *cc <= '9')) {
            json = readNumber();
        }
        else {
            throw RuntimeError("Unexcpected char '%c' at %d,%d. Excpected start of json element", *cc, line(), column());
        }
    }

    const char* JsonReader::readString()
    {
        if (*cc != '"') {
            throw RuntimeError("Expected start of string at %d,%d", line(), column()); 
        }
        cc++;

        const char* stringStart = cc;
        while (*cc != '"') {
            if (*cc == '\0' || (*cc++ == '\\' && *cc++ == '\0')) {
                throw RuntimeError("Runaway string");
            }
        }
        
        char* result = strndup(stringStart, cc - stringStart);
        replaceEscapes(result);
        skip('"');
        return result;

    }

    void JsonReader::replaceEscapes(char* string)
    {
        static char* escapedChar = initializeEscapeTable();
        char* dest = string;
        do {
            if (*string == '\\') {
                string++;
                if (*string == 'u') {
                    readUnicodeEscape(dest, string);
                }
                else if (escapedChar[*string] == '\0') {
                    throw RuntimeError("Unescapable char");
                }
                else {
                    *dest++ = escapedChar[*string++];
                }
            }
            else {
                *dest++ = *string++;
            }
        } 
        while (*string);
        *dest = '\0';
    }

    void JsonReader::readUnicodeEscape(char*& dest, char*& src)
    {
        src++; // Skip 'u'
        uint16_t codePoint = (hexValue(*src++) << 12);
        codePoint += (hexValue(*src++) << 8);
        codePoint += (hexValue(*src++) << 4);
        codePoint += hexValue(*src);

        int returnValue;
        if (codePoint > 0x7FF) {
            *dest++ = (char) (0xE0 | ((codePoint & 0xF000) >> 12));
            *dest++ = (char) (0x80 | ((codePoint & 0xFC0) >> 6));
            *dest++ = (char) (0x80 | (codePoint & 0x3F));
            returnValue = 3;
        }
        else if (codePoint > 0x7F) {
            *dest++ = (char) (0xC0 | ((codePoint & 0x7C0) >> 6));
            *dest++ = (char) (0x80 | (codePoint & 0x3F));
            returnValue = 2;
        }
        else {
            *dest++ = (char) codePoint;
            returnValue = 1;
        }
    }


    double JsonReader::readNumber()
    {
        // The problem with C's built in strtod atof and such is that the parse according to 
        // locale, so the decimal point is expected to be ',' when locale is - eg.- ' da_DK.UTF-8. 
        // Thats not legal in json, hence this handcoded number reader
        
        uint64_t tmp = 0;
        uint64_t digits = 0;
        bool neg = false;
        bool negExp = false;
        int exp = 0;

        if (*cc == '-') {
            neg = true;
            cc++;
        }
    
        // Read integer part
        while (*cc >= '0' && *cc <= '9') {
            if (++digits < 17) { 
                tmp = 10*tmp + (*cc - '0');
            }
            else {
                exp++;
            }
            cc++;
        }

        // Read fraction
        if (*cc == '.') {
            cc++;
            while (*cc >= '0' && *cc <= '9') {
                if (++digits < 17) {
                    tmp = 10*tmp + (*cc - '0');
                    exp--;
                }
                cc++;
            }
            if (*(cc - 1) == '.') {
                throw RuntimeError("Malformed number at %d,%d: Empty fraction", line(), column());
            }
        }

        // Read exponent
        if (*cc == 'E' || *cc == 'e') {
            cc++;
            if (*cc == '-' || *cc == '+') {
                negExp = (*cc++ == '-');
            }
            int tempExp = 0;
            while (*cc >= '0' && *cc <= '9') {
                tempExp = 10*tempExp + (*cc++ - '0');
            }
            if (*(cc - 1) < '0' || *(cc - 1) > '9') {
                throw RuntimeError("Malformed number at %d,%d: empty exponent", line(), column());
            }
            exp = negExp ? exp - tempExp : exp + tempExp;
        } 
            
        skipSpace();
        return neg ? -(tmp*pow(10.0, exp)) : tmp*pow(10.0, exp);
    }

    bool JsonReader::currentCharIsWhiteSpace() {
        static bool *table = initializeSpaceTable();
        return table[*cc];
    }

    void JsonReader::skip() {
        cc++;
        skipSpace();
    }

    void JsonReader::skip(char c)
    {
        if (c != *cc) {
            throw RuntimeError("Unexpected char at %d,%d. Got '%c', expected: '%c'", line(), column(), *cc, c);
        }
        skip();
    }

    void JsonReader::skipSpace()
    {
        while (currentCharIsWhiteSpace()) cc++;
    }

    void JsonReader::skipKeyword(const char* string)
    {
        for (const char* c = string; *c; c++) {
            if (*c != *cc++) {
                throw RuntimeError("Unexpected char at %d,%d. Excepected to read '%s'", line(), column(), string);
            }
        }
        skipSpace();
    }

    uint16_t JsonReader::hexValue(char c)
    {
        static uint16_t* hexTable = initializeHexTable();
        int val = hexTable[c];
        if (val > 15) {
            throw RuntimeError("Not a hex digit");
        }
        return val;
    }
    int JsonReader::line() {
        int l = 1; 
        for (const char* c = buf; c < cc; c++) {
            if (*c == '\n') {
                l++;
            }
        }
        return l;
    }

    int JsonReader::column() {
        int lastLineStart = 0;
        for (const char* c = buf; c < cc; c++) {
            if (*c == '\n') {
                lastLineStart = c - buf + 1;
            }
        }
        return (cc - buf) -  lastLineStart; 
    }

}
