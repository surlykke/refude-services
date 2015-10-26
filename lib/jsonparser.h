#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <stdexcept>

#include "errorhandling.h"

#include "jsontypes.h"

namespace org_restfulipc
{
    class JsonParser
    {
    public:
        JsonParser(char* buf, Heap* heap) :
            buf(buf), heap(heap), pos(0), terminateString(false), lineno(1), beginningOfCurrentLine(0) {}
        AbstractJson* read();

    private:
        JsonObject* readObject();
        JsonArray* readArray();
        char* readString();
        char* readNumber();
        void readBoolean(bool val);
        void readNull();

        char escapeChar(char ch);

        void skipSpace() { while (isspace(buf[pos])) advance(); }
        void skipChar(char c);
        char currentChar() { return buf[pos]; }
        void advance();

        inline void reportError(const char* expected, char unexpected)
        {
            std::cerr << "Expected: " << expected << ", got: " << printable(unexpected)
                      << " at pos " << pos << ", line " << lineno << ", column " << (pos - beginningOfCurrentLine) << "\n";
            throw RuntimeError("Json error");
        }
        inline void reportError(char expected, char unexpected) { reportError(printable(expected).data(), unexpected); }

        std::string printable(char c);

        char* buf;
        Heap* heap;
        int pos;
        bool terminateString;
        int lineno;
        int beginningOfCurrentLine;
    };

}

#endif // JSONPARSER_H
