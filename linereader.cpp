#include <string.h>
#include "linereader.h"
#include "errorhandling.h"

namespace org_restfulipc 
{
        
    LineReader::LineReader(std::string filePath) : 
        lineType(), 
        desktopFile(filePath) 
    {
    }
   
    LineReader::~LineReader()
    {
    }

    LineType LineReader::getNextLine()
    {
        if (lineType == LineType::EndOfFile || ! std::getline(desktopFile, line)) {
            lineType = LineType::EndOfFile;
        }
        else {
            pos = 0;
            skip();
        
            if (pos >= line.size() || line[pos] == '#') { 
                return getNextLine();
            }

            if (line[pos] == '[') { // Group heading
                heading = getBracketContents();
                if (pos < line.size()) {
                    throw RuntimeError("Trailing characters : %s", line.data() + pos);
                }
            
                lineType = LineType::Heading; 
            }
            else { // Key value line
                int keyStart = pos;
                while (pos < line.size() && line[pos] != '[' && line[pos] != '=' && !isspace(line[pos])) pos++;
                key = line.substr(keyStart, pos - keyStart);
                
                if (line[pos] == '[') {
                    locale = getBracketContents();
                }
                else {
                    locale = "";
                    skip();
                }
                
                if (line[pos] != '=') {
                    throw RuntimeError("'=' expected, pos = %d", pos);
                }
                pos++;
                value = line.substr(pos);
               
                lineType = LineType::KeyValue;
            }
        }
        return lineType;
    };

    /**
     * Skip chars in expected, if any, one by one.
     * Then skip trailing whitespace. (Also if expected is an empty string.)
     */
    void LineReader::skip(const char* expected)
    {
        for (const char* c = expected; *c;) {
            if (line[pos++] != *c++) {
                throw RuntimeError("Expected: %s", expected);
            }
        }

        while (isspace(line[pos])) pos++;
    };

    std::string LineReader::getBracketContents()
    {
        // Caller must ensure pos points to a '['
        int start = ++pos;
        while (pos < line.size() && line[pos] != ']')  pos++;
        std::string result = line.substr(start, pos - start);
        skip("]");
        return result;
    }

}