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
#include "mimeappslistreader.h"
#include "linereader.h"
#include "errorhandling.h"
#include "utils.h"

namespace org_restfulipc
{


    MimeappsListReader::MimeappsListReader()
    {
    }

    MimeappsListReader::~MimeappsListReader()
    {
    }

    void MimeappsListReader::read(std::string pathToMimeappsListFile)
    {
        AssocMap tmpAssociations;
        AssocMap tmpBlacklist;

        LineReader lineReader(pathToMimeappsListFile);
        while (lineReader.getNextLine() != LineReader::EndOfFile) {
            if (lineReader.lineType != LineReader::Heading) {
                throw RuntimeError("Heading line expected");
            }
            else if (lineReader.heading == "Added Associations") {
                while (lineReader.getNextLine() == LineReader::KeyValue) {
                    add(tmpAssociations[lineReader.key], lineReader.value, blacklist[lineReader.key]);
                }
            }
            else if (lineReader.heading == "Removed Associations") {
                while (lineReader.getNextLine() == LineReader::KeyValue) {
                    add(tmpBlacklist[lineReader.key], lineReader.value);
                }
            }
            else if (lineReader.heading == "Default Applications") {
                while (lineReader.getNextLine() == LineReader::KeyValue) {
                    add(defaults[lineReader.key], lineReader.value);
                }
            }
            else {
                throw RuntimeError("Unknown heading type: %s\n", lineReader.heading);
            }
        }
        associations.insert(tmpAssociations.begin(), tmpAssociations.end());
        blacklist.insert(tmpBlacklist.begin(), tmpBlacklist.end());
    }

    void MimeappsListReader::addAssociation(std::string application, std::string mimetype)
    {
        AppList& bl = blacklist[mimetype];
        AppList& as = associations[mimetype];
        if (find(bl.begin(), bl.end(), application) == bl.end() && find(as.begin(), as.end(), application) == as.end()) {
            as.push_back(application);
        }
    }

    void MimeappsListReader::add(AppList& lst, string applications, const AppList& excludes)
    {
        for (string application : split(applications, ';')) {
            if (find(excludes.begin(), excludes.end(), application) == excludes.end() &&
                find(lst.begin(), lst.end(), application) == lst.end()) {
                lst.push_back(application);
            } 
        }

    }


}