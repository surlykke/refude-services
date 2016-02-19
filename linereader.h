#ifndef LINEREADER_H
#define LINEREADER_H
#include <string>
#include <fstream>

/** 
 * Simple reader which turns a file of format 'ini' into a stream of lines - group headings and 
 * key-value lines
 * 
 * In terms of lines of code, this could have been done much simpler with regular expressions, 
 * but I found that both compilation time and execution time took a major hit using std::regex.
 */
namespace org_restfulipc 
{
   
    class LineReader 
    {

     public:
        enum LineType {
            Heading,
            KeyValue,
            EndOfFile
        };
        
        LineReader(std::string filePath);
        virtual ~LineReader();
        LineType getNextLine();
        std::string heading;
        std::string key;
        std::string locale;
        std::string value;
        LineType lineType; 

    private:
        std::string getBracketContents();
        void skip(const char *expected = "") ;

        std::ifstream desktopFile;
        std::string line;
        int pos;

    };
}
#endif /* LINEREADER_H */

