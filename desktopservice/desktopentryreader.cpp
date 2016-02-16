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
            json(JsonConst::EmptyObject),
            lines(applicationsDirPath + "/" + relativeFilePath)
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
        lines.getNextLine(); 
        if (lines.lineType != LineType::MainHeading) {
            throw RuntimeError("'[Desktop Entry]' expected");
        }
        readKeyValues(json);

        while (lines.lineType != LineType::EndOfFile) {
            if (lines.lineType == LineType::ActionHeading) {
                if (json["Actions"].undefined() || json["Actions"][lines.action].undefined()) {
                    throw RuntimeError("Unknown action: %s", lines.action.data());
                }
                readKeyValues(json["Actions"][lines.action]);
            }
            else if (lines.lineType == LineType::OtherHeading) {
                if (json["Other_groups"].undefined()) {
                    json["Other_groups"] = JsonConst::EmptyObject;
                }
                json["Other_groups"][lines.customHeading] = JsonConst::EmptyObject;
                readKeyValues(json["Other_groups"][lines.customHeading]);
            }
            else {
                throw RuntimeError("Group heading expected");
            }
        }
    } 


    void DesktopEntryReader::readKeyValues(Json& json) {
        while (lines.getNextLine() == LineType::KeyValue) {
            readKeyValue(json);
        }
    }


    bool DesktopEntryReader::readKeyValue(Json& json) 
    {
        if (keyOneOf({"Type", "Version", "Exec", "Path", "StartupWMClass", "URL"})) {
            json[lines.key] = lines.value;
        }
        else if (keyOneOf({"Name", "GenericName", "Comment"})) {
            if (json[lines.key].undefined())  {
                json[lines.key] = JsonConst::EmptyObject;
            }
            json[lines.key][lines.locale] = lines.value;
        }
        else if (keyOneOf({"NoDisplay", "DBusActivatable", "Terminal", "StartupNotify"})) {
            if (lines.value == "true") { 
                json[lines.key] = JsonConst::TRUE;
            }
            else if (lines.value == "false") {
                json[lines.key] = JsonConst::FALSE;
            }
            else {
                throw RuntimeError("Value for must be 'true' or 'false'");
            }
        }
        else if (keyOneOf({"OnlyShowIn","NotShowIn","MimeType","Categories","Implements"})) {
            json[lines.key] = JsonConst::EmptyArray;
            for (std::string val : toList(lines.value)) {
                json[lines.key].append(val);
            }
        }    
        else if (lines.key == "Keywords") { 
            if (json[lines.key].undefined())  {
                json[lines.key] = JsonConst::EmptyObject;
            }
            json[lines.key][lines.locale] = JsonConst::EmptyArray;
            for (std::string val : toList(lines.value)) {
                json[lines.key][lines.locale].append(val);
            }
        } 
        else if (lines.key == "Actions") {
            json["Actions"] = JsonConst::EmptyObject;
            for (std::string val : toList(lines.value)) {
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
        for (std::string val : list) if (lines.key == val) return true;
        return false;
    }

}