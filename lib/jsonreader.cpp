#include <math.h>
#include <bitset>
#include "jsonreader.h"

namespace org_restfulipc
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
        table['"'] = '/';
        table['t'] = '\t';
        table['n'] = '\n';
        table['b'] = '\b';
        table['r'] = '\r';
        table['f'] = '\f';
        return table;
    }

    JsonReader::JsonReader(char *buf) :
        buf(buf),
        bufferPos(0)
    {
    }

    Json JsonReader::read()
    {
        skipSpace();
        Json json;
        readNext(json);

        if (currentChar() != '\0') {
            throw RuntimeError("Trailing characters");
        }
        return json;
    }

    void JsonReader::readNext(Json& json)
    {
        char cc = currentChar();
        if (cc == '{') {
            json = JsonConst::EmptyObject;
            skip();
            if (currentChar() != '}') {
                for(;;) {
                    const char* entryKey = readString();
                    skip(':');
                    json[entryKey] = read();
                    if (currentChar() != ',') break;
                    skip();
                    if (currentChar() == '}') break;
                }
            }
            skip('}');
        }
        else if (cc == '[')  {
            json = JsonConst::EmptyArray;
            skip();
            if (currentChar () != ']') {
                for (;;) {
                    json.append(std::move(read()));
                    if (currentChar() != ',') break;
                    skip();
                    if (currentChar() == ']') break;
                }
            }
            skip(']');
        }
        else if (cc == '"') {
            json = readString();
        }
        else if (cc = 't') {
            skip("true");
            json = JsonConst::TRUE;
        }
        else if (cc == 'f') {
            skip("false");
            json = JsonConst::FALSE;
        }
        else if (cc == 'n') {
            skip("null");
            json = JsonConst::Null;
        }
        else if (cc == '+' || cc == '-' || (cc >= '0' && cc <= '9')) {
            json = readNumber();
        }
        else {
            throw UnexpectedChar(currentChar(), bufferPos);
        }
    }

    char* JsonReader::readString()
    {
        static char* escapedChar = initializeEscapeTable();
        if (currentChar() != '"') {
            throw UnexpectedChar('"', bufferPos);
        }
        bufferPos++;

        uint32_t stringStart = bufferPos;
        uint32_t stringPos = bufferPos;

        for (;;) {
           switch (currentChar()) {
           case '"':
               buf[stringPos] = '\0';
               skip();
               return buf + stringStart;
               break;
           case '\0':
               throw RuntimeError("Runaway string");
           case '\\':
               bufferPos++;
               if (currentChar() == 'u') {
                   stringPos += readUnicodeEscape(stringPos);
               }
               else if (escapedChar[currentChar()] == '\0') {
                  throw UnescapableChar(currentChar(), bufferPos);
               }
               else {
                   buf[stringPos++] = escapedChar[currentChar()];
               }
               break;
           default:
               buf[stringPos++] = currentChar();
           }
           bufferPos++;
        }

    }

    double JsonReader::readNumber()
    {
        char *endPtr;
        double number = strtod(buf + bufferPos, &endPtr);
        if (number == HUGE_VAL || number == -HUGE_VAL) throw RuntimeError("Overflow");
        bufferPos = endPtr - buf;
        skipSpace();
        return number;
    }

    char JsonReader::currentChar() { return buf[bufferPos]; }

    bool JsonReader::currentCharIsWhiteSpace() {
        static bool *table = initializeSpaceTable();
        return table[currentChar()];
    }

    void JsonReader::skip() {
        bufferPos++;
        skipSpace();
    }

    void JsonReader::skip(char c)
    {
        if (c != currentChar()) {
            throw UnexpectedChar(c, bufferPos);
        }
        skip();
    }

    void JsonReader::skipSpace()
    {
        while (currentCharIsWhiteSpace()) bufferPos++;
    }

    void JsonReader::skip(const char* string)
    {
        for (const char* c = string; *c; c++) {
            if (*c != buf[bufferPos++]) {
                throw UnexpectedChar(*c, bufferPos);
            }
        }
        while (currentCharIsWhiteSpace()) bufferPos++;
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

    int JsonReader::readUnicodeEscape(uint32_t stringPos)
    {
        bufferPos++;
        uint16_t codePoint = (hexValue(buf[bufferPos++]) << 12);
        codePoint += (hexValue(buf[bufferPos++]) << 8);
        codePoint += (hexValue(buf[bufferPos++]) << 4);
        codePoint += hexValue(buf[bufferPos]);

        int returnValue;
        if (codePoint > 0x7FF) {
            buf[stringPos++] = (char) (0xE0 | ((codePoint & 0xF000) >> 12));
            buf[stringPos++] = (char) (0x80 | ((codePoint & 0xFC0) >> 6));
            buf[stringPos++] = (char) (0x80 | (codePoint & 0x3F));
            returnValue = 3;
        }
        else if (codePoint > 0x7F) {
            buf[stringPos++] = (char) (0xC0 | ((codePoint & 0x7C0) >> 6));
            buf[stringPos++] = (char) (0x80 | (codePoint & 0x3F));
            returnValue = 2;
        }
        else {
            buf[stringPos++] = (char) codePoint;
            returnValue = 1;
        }

        return returnValue;
    }


}
