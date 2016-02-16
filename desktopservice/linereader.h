#ifndef LINEREADER_H
#define LINEREADER_H
#include <string>
#include <fstream>

/** 
 * Simple desktop entry reader which turns a desktopfile into a stream of lines. 
 * A line has a LineType and attributes corresponding to that.
 * 
 * In terms of lines of code, this could have been done much simpler with regular expressions, 
 * but I found that both compilation time and execution time took a major hit using std::regex.
 */

namespace org_restfulipc 
{
    enum class LineType {
        Unknown,
        MainHeading,
        ActionHeading,
        OtherHeading,
        KeyValue,
        EndOfFile
    };
    
    class LineReader 
    {
    public:
        LineReader(std::string filePath);
        virtual ~LineReader();
        LineType getNextLine();
        std::string action; 
        std::string customHeading;
        std::string key;
        std::string locale;
        std::string value;
        LineType lineType; 

    private:
        void getHeadingLine();
        void getKeyValueLine();
        std::string getKey();

        void skip(const char *expected = "") ;
        void assertAtEnd();

        std::ifstream desktopFile;
        std::string line;
        int pos;

    };
}
#endif /* LINEREADER_H */

