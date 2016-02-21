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
#include "inireader.h"
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
        MimeAppMap tmpAssociations;
        MimeAppMap tmpBlacklist;
        
        IniReader reader(pathToMimeappsListFile);
        while (reader.getNextLine() != IniReader::EndOfFile) {
            if (reader.lineType != IniReader::Heading) {
                throw RuntimeError("Heading line expected");
            }
            else if (reader.heading == "Added Associations") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    add(tmpAssociations[reader.key], reader.value, blacklist[reader.key]);
                }
            }
            else if (reader.heading == "Removed Associations") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    add(tmpBlacklist[reader.key], reader.value);
                }
            }
            else if (reader.heading == "Default Applications") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    add(defaults[reader.key], reader.value);
                }
            }
            else {
                throw RuntimeError("Unknown heading type: %s\n", reader.heading);
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