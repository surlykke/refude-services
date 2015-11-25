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

    enum action {
        object = 1,
        array,
        string,
        booleanTrue,
        booleanFalse,
        number,
        nil,
        error
    };

    action* initializeActionTable() {
        action* table = new action[256] { error };
        table['{'] = object;
        table['['] = array;
        table['"'] = string;
        table['t'] = booleanTrue;
        table['f'] = booleanFalse;
        table['+'] = table['-'] = number;
        for (int i = '0'; i <= '9'; i++) {
            table[i] = number;
        }
        table['n'] = nil;
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

    void JsonReader::read(Json* json)
    {
        skipSpace();
        readAny(json);
        if (currentChar() != '\0') {
            throw RuntimeError("Trailing characters");
        }
    }

    void JsonReader::readAny(Json* json)
    {
        switch(currentChar()) {
        case '{':
            json->mType = JsonType::Object;
            json->firstEntry = readObject();
            break;
        case '[':
            json->mType = JsonType::Array;
            json->firstElement = readArray();
            break;
        case '"':
            json->mType = JsonType::String;
            json->string = readString();
            break;
        case 't':
            skip("true");
            json->mType = JsonType::Boolean;
            json->boolean = true;
            break;
        case 'f':
            skip("false");
            json->mType = JsonType::Boolean;
            json->boolean = false;
            break;
        case 'n':
            skip("null");
            json->mType = JsonType::Null;
            break;
        case '+':
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            readNumber(json);
            break;
        default:
            throw UnexpectedChar(currentChar(), bufferPos);
        }
     }


    Entry* JsonReader::readObject()
    {
        Entry* firstEntry = NULL;
        skip('{');
        if (currentChar() != '}') {
            Entry* entry = firstEntry = readEntry();
            while (currentChar() == ',') {
                skip();
                if (currentChar() == '}') {
                    break;
                }
                entry->next = readEntry();
                entry = entry->next;
            }
        }
        skip('}');
        return firstEntry;

    }

    Entry* JsonReader::readEntry()
    {
        Entry* entry = new Entry();
        entry->key = readString();
        skip(':');
        readAny(entry);
        return entry;
    }


     Element *JsonReader::readArray()
     {
         Element* firstElement = NULL;
         skip('[');
         if (currentChar() != ']') {
             Element* element = firstElement = readElement();
             while (currentChar() == ',') {
                 skip();
                 if (currentChar() == ']') {
                     break;
                 }
                 element->next = readElement();
                 element = element->next;
             }
         }
         skip(']');
         return firstElement;
     }

     Element *JsonReader::readElement()
     {
         Element* element = new Element();
         readAny(element);
         return element;
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


    void JsonReader::readNumber(Json* json)
    {
        long longResult = 0;
        double doubleResult;
        bool negative = false;
        if (currentChar() == '+') {
            bufferPos++;
        }
        else if (currentChar() == '-') {
            negative = true;
            bufferPos++;
        }

        int mark = bufferPos;

        while (isdigit(currentChar())) {
            longResult = 10*longResult + currentChar() - '0';
            if (longResult < 0) {
                throw RuntimeError("Overflow");
            }
            bufferPos++;
        }

        if (bufferPos <= mark) {
            throw RuntimeError("Expected digit");
        }

        if (currentChar() != '.') {
            skipSpace();
            json->mType = JsonType::Long;
            json->numberL = longResult;
            return;
        }

        bufferPos++;
        double fraction = 0;
        double weight = 1.0;
        while (isdigit(currentChar())) {
            weight = weight*0.1;
            fraction = fraction + weight*(currentChar() - '0');
            bufferPos++;
        }

        doubleResult = longResult + fraction;

        if (currentChar() == 'e' || currentChar() == 'E') {
            bufferPos++;
            double exponent = 0;
            bool negativeExponent = false;
            if (currentChar() == '+') {
                bufferPos++;
            }
            else if (currentChar() == '-') {
                negativeExponent = true;
                bufferPos++;
            }

            int startOfExponentPos = bufferPos;

            while(isdigit(currentChar())) {
                exponent = 10*exponent + currentChar() - '0';
                bufferPos++;
            }

            if (bufferPos <= startOfExponentPos) {
                throw RuntimeError("Expected digit");
            }

            if (negativeExponent) {
                exponent = -exponent;
            }

            doubleResult = doubleResult*pow(10, exponent);
        }
        skipSpace();
        json->mType = JsonType::Double;
        json->numberD = doubleResult;
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
