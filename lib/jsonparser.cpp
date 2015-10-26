#include <string.h>

#include "errorhandling.h"

#include "jsonparser.h"

namespace org_restfulipc
{

    AbstractJson* JsonParser::read()
    {
        skipSpace();
        if ('{' == currentChar()) {
            return readObject();
        }
        else if ('[' == currentChar()) {
            return readArray();
        }
        else if ('"' == currentChar()) {
            return new (heap->allocate<JsonString>()) JsonString(readString());
        }
        else if (currentChar() == '-' || currentChar() == '+' || isdigit(currentChar())) {
            return new (heap->allocate<JsonNumber>()) JsonNumber(readNumber());
        }
        else if (strncmp("true", buf + pos, 4) == 0) {
            pos += 4;
            return new (heap->allocate<JsonBoolean>()) JsonBoolean(true);
        }
        else if (strncmp("false", buf + pos, 5) == 0) {
            pos += 5;
            return new (heap->allocate<JsonBoolean>()) JsonBoolean(false);
        }
        else if (strncmp("null", buf + pos, 4) == 0) {
            pos += 4;
            return new (heap->allocate<JsonString>()) JsonString(readString());
        }
    }

    JsonObject *JsonParser::readObject()
    {
        JsonObject* object = new (heap->allocate<JsonObject>()) JsonObject();
        advance();

        for (;;) {
            skipSpace();
            if ('}' == currentChar()) {
                break;
            }
            const char* key = readString();
            skipSpace();
            skipChar(':');
            skipSpace();
            object->put(key, read(), heap);
            skipSpace();
            if (',' != currentChar()) {
                break;
            }
            skipChar(',');
        }

        skipChar('}');
        return object;
    }

    JsonArray *JsonParser::readArray()
    {
        JsonArray* array = new (heap->allocate<JsonArray>()) JsonArray();
        advance();

        for (;;) {
            skipSpace();
            if (']' == currentChar()) {
                break;
            }
            AbstractJson* element = read();
            array->append(element, heap);
            skipSpace();
            if (',' != currentChar()) {
                break;
            }
            skipChar(',');
        }

        skipChar(']');
        return array;
    }

    char *JsonParser::readString()
    {
       skipChar('"');
        char* string = buf + pos;
        int displacement = 0;
        for(; currentChar() != '"'; advance()) {
            if (currentChar() == '\\') {
                advance();
                displacement++;
                buf[pos] = escapeChar(buf[pos]);
            }
            buf[pos - displacement] = buf[pos];
        }
        terminateString = true;
        skipChar('"');
        return string;
    }

    char *JsonParser::readNumber()
    {
        char* number = buf + pos;
        if (currentChar() == '+' || currentChar() == '-') {
            advance();
        }

        if (!isdigit(currentChar())) {
           reportError("digit expected", currentChar());
        }

        advance();

        while (isdigit(currentChar())) pos++;

        if ('.' == currentChar()) {
            advance();

            while (isdigit(currentChar())) {
                advance();
            }
        }

        if (currentChar() == 'e' || currentChar() == 'E') {
            advance();

            if (currentChar() == '+' || currentChar() == '-') {
                advance();
            }

            if (!isdigit(currentChar())) {
                reportError("Digit expected", currentChar());
            }

            advance();

            while(isdigit(currentChar())) {
                advance();
            }
        }
        terminateString = true;
        return number;
    }

    char org_restfulipc::JsonParser::escapeChar(char ch)
    {
        static const char escapeChars[8]  = {'"', '/', '\\' , 'b', 'f', 'n', 'r', 't'};
        static const char escapedChars[8] = {'"', '/', '\\', '\b', '\f', '\n', '\r', '\t'};
        static const int numEscapedChars = 8;

        for (int i = 0; i < numEscapedChars; i++) {
            if (ch == escapeChars[i]) {
                return escapedChars[i];
            }
        }

        reportError("an escapable char", currentChar());
    }

    void JsonParser::skipChar(char c) {
        if (c != buf[pos]) {
            reportError(c, buf[pos]);
        }
        advance();
    }

    void JsonParser::advance()
    {
        if (currentChar() == '\0') {
            reportError("more text", '\0');
        }

        if (currentChar() == '\n') {
            lineno++;
            beginningOfCurrentLine = pos + 1;
        }

        if (terminateString) {
            buf[pos] = '\0';
            terminateString = false;
        }
        pos++;
    }



    std::string JsonParser::printable(char c) {
        char rep[20];

        if (isprint(c)) {
            sprintf(rep, "'%c'", c);
        }
        else {
            sprintf(rep, "(\\%d)", c);
        }

        return std::string(rep);
    }
}


