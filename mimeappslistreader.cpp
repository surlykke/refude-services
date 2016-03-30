/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mimeappslistreader.cpp
 * Author: Christian Surlykke
 * 
 * Created on 18. februar 2016, 10:38
 */
#include <algorithm>
#include <ripc/errorhandling.h>
#include "mimeappslistreader.h"
#include "inireader.h"
#include <ripc/utils.h>

namespace org_restfulipc
{
    template<typename AppSomething> 
    void writeHlp(ostream& out, string header, AppSomething& appSomething) 
    {
        out << header << "\n";
        for (const auto& it : appSomething) {
            out << it.first << "=";
            for (string entryId : it.second) {
                out << entryId << ";";
            }
            out << "\n";
        }
    }


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
        writeHlp(stream, "[Default Applications]", defaultApps); 
        writeHlp(stream, "[Added Associations]", addedAssociations);
        writeHlp(stream, "[Removed Associations]", removedAssociations);
    }
}