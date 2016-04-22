/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef INIREADER_H
#define INIREADER_H
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
   
    class IniReader 
    {

     public:
        enum LineType {
            AtStart,
            Heading,
            KeyValue,
            EndOfFile
        };
        
        IniReader(std::string filePath);
        virtual ~IniReader();
        virtual LineType getNextLine();
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
#endif /* INIREADER_H */

