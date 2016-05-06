/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <algorithm>
#include <ripc/errorhandling.h>
#include "mimeappslistreader.h"
#include "inireader.h"
#include <ripc/utils.h>

namespace org_restfulipc
{
    MimeappsList::MimeappsList(std::string path):
    filePath(path)
    {
        IniReader reader(filePath);
        while (reader.getNextLine() != IniReader::EndOfFile) {
            if (reader.lineType != IniReader::Heading) {
                throw RuntimeError("Heading line expected");
            }
            else if (reader.heading == "Added Associations") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    addedAssociations[reader.key]  = splitToSet(reader.value, ';');
                }
            }
            else if (reader.heading == "Removed Associations") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    removedAssociations[reader.key] = splitToSet(reader.value, ';');
                }
            }
            else if (reader.heading == "Default Applications") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    defaultApps[reader.key] = split(reader.value, ';');
                }
            }
            else {
                throw RuntimeError("Unknown heading type: %s\n", reader.heading);
            }
        }
    }

    void MimeappsList::write()
    {
        std::ofstream stream(filePath);
    
        stream << "[Default Applications]\n";
        defaultApps.each([&stream](const char* mimetype, vector<string>& defaultApps){
            stream << mimetype << "=";
            for (string defaultApp : defaultApps) {
                stream << defaultApp << ";";
            }
        });

        stream << "[Added Associations]\n";
        addedAssociations.each([&stream](const char* mimetype, set<string>& associatedApps){
            stream << mimetype << "=";
            for (string associatedApp : associatedApps) {
                stream << associatedApp << ";";
            }
        });

        stream << "[Removed Associations]\n";
        removedAssociations.each([&stream](const char* mimetype, set<string>& deAssociatedApps){
            stream << mimetype << "=";
            for (string deAssociatedApp : deAssociatedApps) {
                stream << deAssociatedApp << ";";
            }
        });
    }
}