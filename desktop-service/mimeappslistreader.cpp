/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <algorithm>
#include "errorhandling.h"
#include "mimeappslistreader.h"
#include "inireader.h"
#include "utils.h"

namespace refude
{
    MimeappsList::MimeappsList(std::string path):
    filePath(path)
    {
        IniReader reader(filePath);
        reader.getNextLine();
        while (reader.lineType != IniReader::EndOfFile) {
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
        for (const auto& entry : defaultApps) {
            const std::string& mimetype = entry.key;
            const std::vector<std::string>& defaultApps = entry.value;
                stream << mimetype << "=";
                for (std::string defaultApp : defaultApps) {
                    stream << defaultApp << ";";
                }
                stream << "\n";
        };

        stream << "[Added Associations]\n";
        for (const auto& entry : addedAssociations) {
            const std::string& mimetype = entry.key;
            const std::set<std::string>& associatedApps = entry.value;
                stream << mimetype << "=";
                for (std::string associatedApp : associatedApps) {
                    stream << associatedApp << ";";
                }
                stream << "\n";
        };

        stream << "[Removed Associations]\n";
        for (const auto& entry : removedAssociations) {
            const std::string& mimetype = entry.key;
            const std::set<std::string>& deAssociatedApps = entry.value;
            stream << mimetype << "=";
            for (std::string deAssociatedApp : deAssociatedApps) {
                stream << deAssociatedApp << ";";
            }
            stream << "\n";
        };
    }
}
