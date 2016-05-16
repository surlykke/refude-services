/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <iostream>
#include <regex>
#include <ctype.h>
#include <ripc/errorhandling.h>
#include <ripc/json.h>
#include <ripc/jsonwriter.h>

#include "desktopentryreader.h"
#include "desktopTemplate.h"
#include "desktopActionTemplate.h"

namespace org_restfulipc 
{

    DesktopEntryReader::DesktopEntryReader(std::string desktopFilePath) : 
        IniReader(desktopFilePath),
        json(JsonConst::EmptyObject)
    {
        json << desktopTemplate_json;
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
                json["Other_groups"][heading] << desktopActionTemplate_json;
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
            if (locale.empty()) {
                json[key]["_ripc:localized"] = value;
            }
            else {
                json[key][locale] = value;
            }
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
                json[key].append(val);
            }
        }    
        else if (key == "Keywords") { 
            Json keywords = JsonConst::EmptyArray; 
          
            for (std::string keyword : toList(value)) {
                keywords.append(keyword);
            }
        
            if (locale.empty()) {
                json[key]["_ripc:localized"] = std::move(keywords);
            }
            else {
                json[key][locale] = std::move(keywords);
            }
        } 
        else if (key == "Actions") {
            json["Actions"] = JsonConst::EmptyObject;
            for (std::string val : toList(value)) {
                json["Actions"][val] << desktopActionTemplate_json;
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