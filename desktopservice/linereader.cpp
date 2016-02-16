#include <string.h>
#include "linereader.h"
#include "errorhandling.h"

namespace org_restfulipc 
{
        
    LineReader::LineReader(std::string filePath) : 
        lineType(LineType::Unknown), 
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

            if (line[pos] == '[') {
                getHeadingLine();    
            }
            else {
                getKeyValueLine();
            }
        }
        return lineType;
    };

    void LineReader::getHeadingLine() {
        int startOfHeading = pos + 1;
        skip("[");
        if (!strncmp("Desktop", line.data() + pos, 7)) {
            // We assume a group heading starting with 'Desktop' is either 'Desktop Entry' or 
            // 'Desktop Action <action>'. Ie. we do not allow custom group headings starting with 'Desktop'
            skip("Desktop");
            if (line[pos] == 'E') {
                skip("Entry");
                skip("]");
                lineType = LineType::MainHeading;
            }
            else {
                skip("Action");
                action = getKey();
                skip("]");
                lineType = LineType::ActionHeading;
            }
        }
        else {
            while (pos < line.size() && line[pos] != ']') {
                pos++;
            }
            customHeading = line.substr(startOfHeading, pos - startOfHeading);
            skip("]");
            lineType = LineType::OtherHeading; 
        }
        assertAtEnd();
    }

    void LineReader::getKeyValueLine() {
        key = getKey();
        if (line[pos] == '[') {
            int localeStart = ++pos;
            while (line[pos] != ']') {
                if (pos >= line.size()) throw RuntimeError("']' expected");
                pos++;
            }
            locale = line.substr(localeStart, pos++ - localeStart);
        }
        else {
            locale = "";
        }
        skip("");
        skip("=");
        size_t end = line.size();
        while (isspace(line[end]) && end > pos) end--;
        value = line.substr(pos, end - pos);
        lineType = LineType::KeyValue;
    }

    
    
    void LineReader::skip(const char* expected)
    {
        for (const char* c = expected; *c;) {
            if (line[pos++] != *c++) {
                throw RuntimeError("Expected: %s", expected);
            }
        }

        while (isspace(line[pos])) pos++;
    };

    void LineReader::assertAtEnd()
    {
        if (line.size() > pos) throw RuntimeError("Trailing characters: %s\n", line.substr(pos).data());
    };

    std::string LineReader::getKey() 
    {
        int keyStart = pos;
        
        while (pos < line.size() && (isalpha(line[pos]) || line[pos] == '-')) {
            pos++;
        }

        return  line.substr(keyStart, pos - keyStart);
    }
 



}