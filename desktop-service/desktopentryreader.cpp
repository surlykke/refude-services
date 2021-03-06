/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <iostream>
#include <regex>
#include <ctype.h>
#include "errorhandling.h"
#include "json.h"
#include "jsonwriter.h"

#include "desktopentryreader.h"

namespace refude 
{

    const char* desktopTemplate = R"json(
        {
            "_ripc:localized:Name" : {
                "" : true
            },
            "_ripc:localized:GenericName" : { }, 
            "_ripc:localized:Comment" : { }, 
            "_ripc:localized:Keywords" : { },
            "OnlyShowIn" : [],
            "NotShowIn" : [],
            "mimetype" : [],
            "Categories" : [],
            "Implements" : []
        }
    )json";

    const char* desktopActionTemplate = R"json(
        { 
            "_ripc:localized:Name" : {}
        }
    )json";
 
    std::string camelize(std::string str) 
    {
        if (str == "MimeType") {
            return "mimetype";
        }
        else {
            return str; // FIXME
        }
    }


    DesktopEntryReader::DesktopEntryReader(std::string desktopFilePath) : 
        IniReader(desktopFilePath),
        json(JsonConst::EmptyObject)
    {
        json << desktopTemplate;
        read();
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
                json["Other_groups"][heading] << desktopActionTemplate;
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

        if (keyOneOf({"Type", "Version", "Exec", "Path", "StartupWMClass", "URL", "Icon"})) {
            // Strangely, Freedesktop.org's desktop entry spec states 'Icon' is a locale-string
            // That doesn't make any sense?
            json[key] = value;
        }
        else if (keyOneOf({"Name", "GenericName", "Comment"})) {
            std::string _ripc_localized_key = std::string("_ripc:localized:") + key;
            json[_ripc_localized_key][locale] = value;
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
            for (std::string val : toList(value)) {
                json[camelize(key)].append(val);
            }
        }    
        else if (key == "Keywords") { 
            Json keywords = JsonConst::EmptyArray; 
          
            for (std::string keyword : toList(value)) {
                keywords.append(keyword);
            }
        
            json["_ripc:localized:Keywords"][locale] = std::move(keywords);
        } 
        else if (key == "Actions") {
            json["Actions"] = JsonConst::EmptyObject;
            for (std::string val : toList(value)) {
                json["Actions"][val] << desktopActionTemplate;
            }
        }
    }



    std::list<std::string> DesktopEntryReader::toList(std::string value) {
        std::list<std::string> result;
        std::size_t startOfRemaining = 0; 
        std::size_t semicolonPos;
        while ((semicolonPos = value.find(";", startOfRemaining)) != std::string::npos) {
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
