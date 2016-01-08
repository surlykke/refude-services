#ifndef JSONREADER_H
#define JSONREADER_H

#include <stdint.h>
#include "errorhandling.h"
#include "json.h"

namespace org_restfulipc
{
    class JsonReader
    {
    public:
        JsonReader(char* buf);
        Json&& read();

    private:
        void readNext(Json& json);
        char *readString();
        double readNumber();

        char currentChar();
        bool currentCharIsWhiteSpace();

        void skip();
        void skip(char c);
        void skipSpace();
        void skip(const char* string);
        uint16_t hexValue(char c);
        int readUnicodeEscape(uint32_t stringPos);

        char* buf;
        uint32_t bufferPos;

    };

    class UnexpectedChar : public RuntimeError
    {
    public:
        UnexpectedChar(char c, uint32_t bufferPos) : RuntimeError("") {}
    };

    class UnescapableChar : public RuntimeError
    {
    public:
        UnescapableChar(char c, uint32_t bufferPos) : RuntimeError(std::string("Got ") + c + " at " + std::to_string(bufferPos)) {}

    };
}
#endif // JSONREADER_H
