#include <iostream>
#include <regex>
#include <ctype.h>
#include "desktopentryreader.h"
#include "desktopTemplate.h"
#include "errorhandling.h"
#include "json.h"
#include "jsonwriter.h"

namespace org_restfulipc 
{
    enum State 
    {
        atStart = 0,
        inDesktopEntryGroup,
        inActionGroup
    };

    class LineReader 
    {
    public:
        LineReader(std::string filePath) : lineType(Unknown), desktopFile(filePath) {}
        bool getNextLine();
        std::string action; 
        std::string customHeading;
        std::string key;
        std::string locale;
        std::string value;
        enum {
            Unknown,
            MainHeading,
            ActionHeading,
            OtherHeading,
            KeyValue,
            EndOfFile
        } lineType;

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

    bool LineReader::getNextLine()
    {
        if (! std::getline(desktopFile, line)) {
            lineType = EndOfFile;
            return false;
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
            return true;
        }
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
                assertAtEnd();
                lineType = MainHeading;
            }
            else {
                skip("Action");
                action = getKey();
                skip("]");
                lineType = ActionHeading;
                assertAtEnd();
            }
        }
        else {
            while (pos < line.size() && line[pos] != ']') {
                pos++;
            }
            customHeading = line.substr(startOfHeading, pos - startOfHeading);
            skip("]");
            assertAtEnd();
        }

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
        skip("");
        skip("=");
        size_t end = line.size();
        while (isspace(line[end]) && end > pos) end--;
        value = line.substr(pos, end - pos);
        lineType = KeyValue;
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
 
    DesktopEntryReader::DesktopEntryReader(std::string applicationsDirPath, std::string relativeFilePath) :
            json(JsonConst::EmptyObject),
            lineReader(new LineReader(applicationsDirPath + "/" + relativeFilePath))
    {
        json << desktopTemplate_json;
        read();
        
        std::replace(relativeFilePath.begin(), relativeFilePath.end(), '/', '-') ;
        std::string selfHref = std::string("/desktopservice/desktopEntry/") + relativeFilePath;
        json["_links"]["self"]["href"] = selfHref;
    }

    DesktopEntryReader::~DesktopEntryReader() 
    {
    }

    void DesktopEntryReader::read()
    {
        lineReader->getNextLine(); 
        if (lineReader->lineType != LineReader::MainHeading) {
            throw RuntimeError("'[Desktop Entry]' expected");
        }
        readKeyValues(json);

        for (;;) {
            if (lineReader->lineType == LineReader::EndOfFile) {
                break;
            }
            else if (lineReader->lineType == LineReader::ActionHeading) {
                if (json["Actions"].undefined() || json["Actions"][lineReader->action].undefined()) {
                    throw RuntimeError("Unknown action: %s", lineReader->action.data());
                }
                readKeyValues(json["Actions"][lineReader->action]);
            }
            else if (lineReader->lineType == LineReader::OtherHeading) {
                if (json["Other_groups"].undefined()) {
                    json["Other_groups"] = JsonConst::EmptyObject;
                }
                json["Other_groups"][lineReader->customHeading] = JsonConst::EmptyObject;
                readKeyValues(json["Other_groups"][lineReader->customHeading]);
            }
            else {
                throw RuntimeError("Group heading expected");
            }
            
        }
    } 


    void DesktopEntryReader::readKeyValues(Json& json) {
        while (lineReader->getNextLine() && lineReader->lineType == LineReader::KeyValue) {
            readKeyValue(json);
        }
    }


    bool DesktopEntryReader::readKeyValue(Json& json) 
    {
        if (keyOneOf({"Type", "Version", "Exec", "Path", "StartupWMClass", "URL"})) {
            json[lineReader->key] = lineReader->value;
        }
        else if (keyOneOf({"Name", "GenericName", "Comment"})) {
            if (json[lineReader->key].undefined())  {
                json[lineReader->key] = JsonConst::EmptyObject;
            }
            json[lineReader->key][lineReader->locale] = lineReader->value;
        }
        else if (keyOneOf({"NoDisplay", "DBusActivatable", "Terminal", "StartupNotify"})) {
            if (lineReader->value == "true") { 
                json[lineReader->key] = JsonConst::TRUE;
            }
            else if (lineReader->value == "false") {
                json[lineReader->key] = JsonConst::FALSE;
            }
            else {
                throw RuntimeError("Value for must be 'true' or 'false'");
            }
        }
        else if (keyOneOf({"OnlyShowIn","NotShowIn","MimeType","Categories","Implements"})) {
            json[lineReader->key] = JsonConst::EmptyArray;
            for (std::string val : toList(lineReader->value)) {
                json[lineReader->key].append(val);
            }
        }    
        else if (lineReader->key == "Keywords") { 
            if (json[lineReader->key].undefined())  {
                json[lineReader->key] = JsonConst::EmptyObject;
            }
            json[lineReader->key][lineReader->locale] = JsonConst::EmptyArray;
            for (std::string val : toList(lineReader->value)) {
                json[lineReader->key][lineReader->locale].append(val);
            }
        } 
        else if (lineReader->key == "Actions") {
            json["Actions"] = JsonConst::EmptyObject;
            for (std::string val : toList(lineReader->value)) {
                json["Actions"][val] = JsonConst::EmptyObject;
            }
        }
    }



    std::list<std::string> DesktopEntryReader::toList(std::string value) {
        std::list<std::string> result;
        std::size_t startOfRemaining = 0; 
        std::size_t semicolonPos;
        while ((semicolonPos = value.find(";", startOfRemaining)) != string::npos) {
            result.push_back(value.substr(startOfRemaining, semicolonPos - startOfRemaining));
            startOfRemaining = semicolonPos + 1;
        }; 
        return result;
    }
    

    bool DesktopEntryReader::keyOneOf(std::list<std::string> list) {
        for (std::string val : list) if (lineReader->key == val) return true;
        return false;
    }

}