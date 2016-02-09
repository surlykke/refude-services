#include <iostream>
#include <list>
#include <fstream>
#include <regex>
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

    enum  ValueType 
    {
        str = 0,
        strlist,
        boolean
    };


    DesktopEntryReader::DesktopEntryReader() :
        json(JsonConst::EmptyObject),
        state(atStart),
        currentAction()
    {
        read("/usr/share/applications/firefox.desktop");
        std::cout << JsonWriter(&json).buffer.data << "\n";
    }

    DesktopEntryReader::~DesktopEntryReader() 
    {
    }

    void DesktopEntryReader::read(const char* desktopFilePath) 
    {
        static std::regex groupHeadingReg("^\\s*\\[\\s*(.*)\\s*\\]\\s*$");
        static std::regex commentLine("^\\s*(#.*)*$");
        static std::regex keyValueLineReg("^\\s*([A-Za-z0-9-]+)(\\[([A-Za-z_]+)\\])?\\s*=\\s*(.*?)\\s*$");

        std::string line;
        std::ifstream desktopFile(desktopFilePath);
        while (std::getline(desktopFile, line)) {
            std::smatch m;
            if (std::regex_match(line, commentLine)) {
            }
            else if (std::regex_match(line, m, groupHeadingReg)) {
                handleGroupHeading(m[1]);
            }
            else if (std::regex_match(line, m, keyValueLineReg)) {
                handleKeyValuePair(m[1], m[3], m[4]);
            }
            else {
                throw RuntimeError("Could not match: '%s'\n", line.data());
            }
        }
    }

    void DesktopEntryReader::handleGroupHeading(std::string heading) 
    {
        std::regex actionHeadingReg("^\\s*Desktop\\s+Action\\s+(\\w*)\\s*$");
        std::smatch m;
        if (state == State::atStart) {
            if (heading == "Desktop Entry") {
                state = inDesktopEntryGroup;
            }
            else {
                throw RuntimeError("Desktop file should start with '[Desktop Entry]'-line");
            }
        }
        else if (std::regex_match(heading, m, actionHeadingReg)) {
            // FIXME check action 
            state = State::inActionGroup;
            currentAction = m[1];
            json[currentAction] = JsonConst::EmptyObject;
        }
    }
    void DesktopEntryReader::handleKeyValuePair(std::string key, std::string locale, std::string value) 
    {
        // FIXME Handle locale strings
        if (state == State::atStart) {
            throw RuntimeError("Desktop file should start with '[Desktop Entry]'-line");
        }
     
        /*if (! ((currentAction.empty() && json[key].mType == JsonType::Undefined) ||
               json[currentAction][key].mType == JsonType::Undefined)) { 
            throw RuntimeError("Duplicate definition of key: %s", key); 
        }*/
        if (currentAction.empty()) { 
            static std::list<std::string> stringKeys = 
                {"Type", "Version","Name", "GenericName", "Comment", "Icon", "Exec", "Path", "StartupWMClass", "URL"};
            static std::list<std::string> boolKeys =  
                {"NoDisplay", "DBusActivatable", "Terminal", "StartupNotify"};
            static std::list<std::string> stringlistKeys = 
                {"OnlyShowIn","NotShowIn","Actions","MimeType","Categories","Implements","Keywords"};

            for (std::string stringKey : stringKeys) {
                if (key == stringKey) {
                    handleStringValue(key, value);
                    return;
                } 
            } 
            for (std::string boolKey : boolKeys) {
                if (key == boolKey) {
                    handleBool(key, value);
                    return;
                } 
            } 
            for (std::string stringlistKey : stringlistKeys) {
                if (key == stringlistKey) {
                    handleStringlistKey(key, value);
                    return;
                } 
            }    
             
        }
    }

    void DesktopEntryReader::handleStringValue(std::string key, std::string value) {
        Json& element = currentAction.empty() ? json[key] : json[currentAction][key];
        element = value;
    }

    void DesktopEntryReader::handleBool(std::string key, std::string value) {
        Json& element = currentAction.empty() ? json[key] : json[currentAction][key];
        if (value == "true") {
            element = JsonConst::TRUE; 
        }
        else if (value == "false") {
            element = JsonConst::FALSE;
        } 
        else {
            throw RuntimeError("Value for key %s must be 'true' or 'false'", key);
        }
    }

    void DesktopEntryReader::handleStringlistKey(std::string key, std::string value) {
        Json& element = currentAction.empty() ? json[key] : json[currentAction][key];
        element = JsonConst::EmptyArray; 
        std::size_t startOfRemaining = 0; 
        std::size_t semicolonPos;
        while ((semicolonPos = value.find(";", startOfRemaining)) != string::npos) {
            element.append(value.substr(startOfRemaining, semicolonPos - startOfRemaining));
            startOfRemaining = semicolonPos + 1;
        }; 
    }

}