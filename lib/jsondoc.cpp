/*
 * File:   json.cpp
 * Author: christian
 * 
 * Created on 23. juli 2015, 10:32
 */

#include <stdlib.h>
#include <sys/time.h>
#include <iomanip>

#include "errorhandling.h"
#include "jsondoc.h"


namespace org_restfulipc
{

    using namespace std;

    typedef uint32_t JsonRef;

    struct Pimpl
    {
        enum JsonTypeTag : uint32_t
        {
            Object   = (uint32_t) 0x20000000,
            Array    = (uint32_t) 0x40000000,
            String   = (uint32_t) 0x60000000,
            Boolean  = (uint32_t) 0x80000000,
            Number   = (uint32_t) 0xA0000000,
            Null     = (uint32_t) 0xC0000000
        };


        const char* typeAsStr(JsonRef jsonRef)
        {
            static const char* names[] =
            {
                "Not a jsonref",
                "Object",
                "Array",
                "String",
                "Boolean",
                "Number",
                "Null"
            };

            return names[jsonRef >> 29];
        }

        const uint32_t tag_mask = 0xE0000000;
        const uint32_t val_mask = 0x1FFFFFFF;

        inline JsonRef type(JsonRef ref) { return tag_mask & ref; }
        inline uint32_t val(JsonRef ref) { return val_mask & ref; }

        inline void typeAssert(JsonTypeTag tag, JsonRef ref) { if (tag != type(ref)) throw RuntimeError("Type mismatch"); }
        inline bool booleanValue(JsonRef ref) { typeAssert(JsonTypeTag::Boolean, ref); return val(ref); }
        inline char* stringValue(JsonRef ref) { typeAssert(JsonTypeTag::String, ref); return buf + val(ref); }
        inline char* numberValue(JsonRef ref) { typeAssert(JsonTypeTag::Number, ref); return buf + val(ref); }

        inline bool isObject(JsonRef ref) { return type(ref) == JsonTypeTag::Object; }
        inline bool isArray(JsonRef ref) { return type(ref) == JsonTypeTag::Array; }
        inline bool isString(JsonRef ref) { return type(ref) == JsonTypeTag::String; }
        inline bool isBoolean(JsonRef ref) { return type(ref) == JsonTypeTag::Boolean; }
        inline bool isNumber(JsonRef ref) { return type(ref) == JsonTypeTag::Number; }
        inline bool isNull(JsonRef ref) { return type(ref) == JsonTypeTag::Null; }

        inline bool isNil(JsonRef ref) { return (JsonTypeTag::Object == type(ref) || JsonTypeTag::Array == type(ref)) &&    val(ref) == 0;}

        inline JsonRef string(uint32_t pos) { return JsonTypeTag::String | pos; }
        inline JsonRef boolean(bool b) { return JsonTypeTag::Boolean | (b & val_mask); }
        inline JsonRef number(uint32_t pos) { return JsonTypeTag::Number | pos; }
        inline JsonRef array(uint32_t pos) { return JsonTypeTag::Array | pos; }
        inline JsonRef object(uint32_t pos) { return JsonTypeTag::Object | pos; }
        inline JsonRef null() { return JsonTypeTag::Null; }


        Pimpl(char* buf) :
            pRoot(0),
            buf(buf),
            bufferPos(0),
            refs(new JsonRef[1000]),
            refs_used(1),
            refs_capacity(1000)
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            __suseconds_t t1 = tv.tv_usec;
            pRoot = read();
            gettimeofday(&tv, NULL);
            std::cout << "Time spent: " << (tv.tv_usec - t1) << "\n";
        }

        JsonRef read();
        JsonRef readObject();
        JsonRef readArray();
        uint32_t readString();
        uint32_t readNumber();

        char *serialize(char *dest, JsonRef ref);
        int serializedLength(JsonRef ref);
        char *writeEscapedString(char *dest, const char* string);
        int escapedStringLength(const char* string);
        char *writeString(char *dest, const char *string);
        char currentChar() { return buf[bufferPos]; }

        void skipChar(char c);
        void advance();
        void skipSpace2();
        char escapeChar(char ch);
        std::string printable(char c);
        std::string buildErrorMessage(std::string expected);
        bool terminateString;


        JsonRef pRoot;
        char *buf;
        uint32_t bufferPos;

        JsonRef* refs;
        uint32_t refs_used;
        uint32_t refs_capacity;

        JsonRef newEntry(uint32_t key, JsonRef value)  {
            if (refs_used > refs_capacity - 3) {
                // FIXME growRefs();
            }
            refs[refs_used++] = key;
            refs[refs_used++] = value;
            refs[refs_used++] = object(0);

            return object(refs_used - 3);
        }

        JsonRef newElement(JsonRef value)  {
            if (refs_used > refs_capacity - 2) {
                // FIXME growRefs();
            }
            refs[refs_used++] = value;
            refs[refs_used++] = object(0);

            return array(refs_used - 2);
        }



    };


    char *Pimpl::serialize(char *dest, JsonRef ref)
    {
        int count = 0;
        switch (type(ref)) {
        case JsonTypeTag::String:
            return writeEscapedString(dest, buf + val(ref));
        case JsonTypeTag::Number:
            return writeString(dest, buf + val(ref));
        case JsonTypeTag::Boolean:
            return writeString(dest, val(ref) ? "true" : "false");
        case JsonTypeTag::Null:
            return writeString(dest, "null");
        case JsonTypeTag::Array:
            *(dest++) = '[';
            if (val(ref)) {
                for (;;) {
                    JsonRef valRef = refs[val(ref)];
                    JsonRef nextRef = refs[val(ref) + 1];
                    dest = serialize(dest, refs[val(ref)]);
                    if (val(nextRef)) {
                        ref = nextRef;
                        *(dest++) = ',';
                        *(dest++) = ' ';
                    }
                    else {
                        break;
                    }
                }
            }

            *(dest++) = ']';
            *dest = '\0';
            return dest;
        case JsonTypeTag::Object:
            *(dest++) = '{';
            if (val(ref))  {
                for (;;) {
                    uint32_t keyRef = refs[val(ref)];
                    JsonRef valRef = refs[val(ref) + 1];
                    JsonRef nextRef = refs[val(ref) + 2];

                    dest = writeString(dest, buf + keyRef);
                    *(dest++) = ':';
                    *(dest++) = ' ';
                    dest = serialize(dest, valRef);
                    if (val(nextRef)) {
                        ref = nextRef;
                        *(dest++) = ',';
                        *(dest++) = ' ';
                    }
                    else {
                        break;
                    }
                }
            }

            *(dest++) = '}';
            *dest = '\0';
            return dest;
        }
    }

   char *Pimpl::writeEscapedString(char *dest, const char *string)
    {
        *(dest++) = '"';
        for (const char* c = string; *c; c++) {
            if (*c == '\\' || *c == '"') { // FIXME Much more here
                *(dest++) = '\\';
            }
            *(dest++) = *c;
        }
        *(dest++) = '"';
        *dest = '\0';

       return dest;
   }

   int Pimpl::escapedStringLength(const char *string)
   {
        int length = 0;
        for (const char* c = string; *c; c++) {
            if (*c == '\\' || *c == '"') {
                length++;
            }
            length++;
        }

        return length + 2;
   }




   char *Pimpl::writeString(char *dest, const char *string)
   {
        const char *c = string;
        while (*c) *(dest++) = *(c++);
        return dest;
   }


    int Pimpl::serializedLength(JsonRef ref)
    {

    }

    JsonRef Pimpl::read()
    {
        JsonRef ref;
        skipSpace2();
        if ('{' == currentChar()) {
            ref = readObject();
        }
        else if ('[' == currentChar()) {
            ref = readArray();
        }
        else if ('"' == currentChar()) {
            ref = string(readString());
        }
        else if (currentChar() == '-' || currentChar() == '+' || isdigit(currentChar())) {
            ref = number(readNumber());
        }
        else if (strncmp("true", buf + bufferPos, 4) == 0) {
            bufferPos += 4;
            ref = boolean(true);
        }
        else if (strncmp("false", buf + bufferPos, 5) == 0) {
            bufferPos += 5;
            ref = boolean(false);
        }
        else if (strncmp("null", buf + bufferPos, 4) == 0) {
            bufferPos += 4;
            ref = null();
        }
        else {
            throw RuntimeError(buildErrorMessage("Valid json type"));
        }

        return ref;
    }

    JsonRef Pimpl::readObject()
    {
        JsonRef object = JsonTypeTag::Object;
        JsonRef *objectRef = &object;
        advance();

        for (;;) {
            skipSpace2();
            if ('}' == currentChar()) {
                break;
            }
            uint32_t keyRef = readString();
            skipSpace2();
            skipChar(':');
            skipSpace2();
            JsonRef entry = newEntry(keyRef, read());
            *objectRef = entry;
            objectRef = refs + val(entry) + 2;
            skipSpace2();
            if (',' != currentChar()) {
                break;
            }
            skipChar(',');
        }

        skipChar('}');
        return object;
    }

    JsonRef Pimpl::readArray()
    {
        JsonRef newArray = JsonTypeTag::Array;
        JsonRef *arrayRef = &newArray;
        advance();

        for (;;) {
            skipSpace2();
            if (']' == currentChar()) {
                break;
            }
            JsonRef element = newElement(read());
            *arrayRef = element;
            arrayRef = refs + val(element) + 1;
            skipSpace2();
            if (',' != currentChar()) {
                break;
            }
            skipChar(',');
        }

        skipChar(']');
        return newArray;
    }


    uint32_t Pimpl::readString()
    {
        skipChar('"');
        uint32_t result = bufferPos;
        int displacement = 0;
        for(; currentChar() != '"'; advance()) {
            if (currentChar() == '\\') {
                advance();
                displacement++;
                buf[bufferPos] = escapeChar(buf[bufferPos]);
            }
            buf[bufferPos - displacement] = buf[bufferPos];
        }
        terminateString = true;
        skipChar('"');
        return result;
    }

    uint32_t Pimpl::readNumber()
    {
        int result = bufferPos;
        if (currentChar() == '+' || currentChar() == '-') {
            advance();
        }

        if (!isdigit(currentChar())) {
           throw RuntimeError(buildErrorMessage("Expected digit"));
        }

        advance();

        while (isdigit(currentChar())) bufferPos++;

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
                throw RuntimeError(buildErrorMessage("Expected digit"));
            }

            advance();

            while(isdigit(currentChar())) {
                advance();
            }
        }
        terminateString = true;
        return result;
    }

    char Pimpl::escapeChar(char ch)
    {
        static const char escapeChars[8]  = {'"', '/', '\\' , 'b', 'f', 'n', 'r', 't'};
        static const char escapedChars[8] = {'"', '/', '\\', '\b', '\f', '\n', '\r', '\t'};
        static const int numEscapedChars = 8;

        for (int i = 0; i < numEscapedChars; i++) {
            if (ch == escapeChars[i]) {
                return escapedChars[i];
            }
        }

        throw RuntimeError(buildErrorMessage("Expected an escapable char"));
    }

    void Pimpl::skipChar(char c) {
        if (c != buf[bufferPos]) {
            throw RuntimeError(buildErrorMessage("" + c));
        }
        advance();
    }

    void Pimpl::advance()
    {
        if (currentChar() == '\0') {
            throw RuntimeError("Unexpected end of input");
        }

        if (terminateString) {
            buf[bufferPos] = '\0';
            terminateString = false;
        }

        bufferPos++;
    }

    void Pimpl::skipSpace2()
    {
        while (isspace(buf[bufferPos])) advance();
    }

    std::string Pimpl::printable(char c) {
        char rep[20];

        if (isprint(c)) {
            sprintf(rep, "'%c'", c);
        }
        else {
            sprintf(rep, "(\\%d)", c);
        }

        return std::string(rep);
    }

    std::string Pimpl::buildErrorMessage(std::string expected)
    {
        int lineNo = 1;
        int startOfLastLine = 0;
        for (int i = 0; i < bufferPos; i++) {
            if (buf[i] == '\n') {
                lineNo ++;
                startOfLastLine = i;
            }
        }

        int columnNo = bufferPos - startOfLastLine + 1;

        return expected + " but got " + buf[bufferPos] + " at line " +
                std::to_string(lineNo) + ", column " + std::to_string(columnNo);
    }

    JsonDoc::JsonDoc(char *buf) : pimpl(new Pimpl(buf)) { }

    void JsonDoc::serialize(char *dest)
    {
        pimpl->serialize(dest, pimpl->pRoot);
    }
}



