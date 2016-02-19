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

    DesktopEntryReader::DesktopEntryReader(std::string applicationsDirPath, std::string relativeFilePath) :
        IniReader(applicationsDirPath + "/" + relativeFilePath),
        json(JsonConst::EmptyObject)
            
    {
        json << desktopTemplate_json;
        read();
        
        entryId = relativeFilePath;
        std::replace(entryId.begin(), entryId.end(), '/', '-') ;
        std::string selfHref = std::string("/desktopentry/") + entryId;
        json["_links"]["self"]["href"] = selfHref;
    }

    DesktopEntryReader::~DesktopEntryReader() 
    {
    }

    void DesktopEntryReader::read()
    {
        getNextLine(); 
        if (lineType != Heading || heading != "Desktop Entry") {
            throw RuntimeError("'[Desktop Entry]' expected");
        }
        readKeyValues(json);

        while (lineType != EndOfFile) {
            if (lineType == Heading && heading.substr(0, 14) == "Desktop Action") {
                std::string action = heading.substr(15);
                if (json["Actions"].undefined() || json["Actions"][action].undefined()) {
                    throw RuntimeError("Unknown action: %s", action.data());
                }
                readKeyValues(json["Actions"][action]);
            }
            else if (lineType == Heading) {
                if (json["Other_groups"].undefined()) {
                    json["Other_groups"] = JsonConst::EmptyObject;
                }
                json["Other_groups"][heading] = JsonConst::EmptyObject;
                readKeyValues(json["Other_groups"][heading]);
            }
            else {
                throw RuntimeError("Group heading expected");
            }
        }
    } 


    void DesktopEntryReader::readKeyValues(Json& json) {
        while (getNextLine() == KeyValue) {
            readKeyValue(json);
        }
    }


    bool DesktopEntryReader::readKeyValue(Json& json) 
    {
        if (keyOneOf({"Type", "Version", "Exec", "Path", "StartupWMClass", "URL"})) {
            json[key] = value;
        }
        else if (keyOneOf({"Name", "GenericName", "Comment"})) {
            if (json[key].undefined())  {
                json[key] = JsonConst::EmptyObject;
            }
            json[key][locale] = value;
        }
        else if (keyOneOf({"NoDisplay", "DBusActivatable", "Terminal", "StartupNotify"})) {
            if (value == "true") { 
                json[key] = JsonConst::TRUE;
            }
            else if (value == "false") {
                json[key] = JsonConst::FALSE;
            }
            else {
                throw RuntimeError("Value for must be 'true' or 'false'");
            }
        }
        else if (keyOneOf({"OnlyShowIn","NotShowIn","MimeType","Categories","Implements"})) {
            json[key] = JsonConst::EmptyArray;
            for (std::string val : toList(value)) {
                json[key].append(val);
            }
        }    
        else if (key == "Keywords") { 
            if (json[key].undefined())  {
                json[key] = JsonConst::EmptyObject;
            }
            json[key][locale] = JsonConst::EmptyArray;
            for (std::string val : toList(value)) {
                json[key][locale].append(val);
            }
        } 
        else if (key == "Actions") {
            json["Actions"] = JsonConst::EmptyObject;
            for (std::string val : toList(value)) {
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
        for (std::string val : list) if (key == val) return true;
        return false;
    }

}