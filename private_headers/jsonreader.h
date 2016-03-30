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
