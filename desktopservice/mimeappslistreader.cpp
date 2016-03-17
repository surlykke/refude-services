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
    MimeappsList::MimeappsList(std::string path)
    {
        IniReader reader(path);
        while (reader.getNextLine() != IniReader::EndOfFile) {
            if (reader.lineType != IniReader::Heading) {
                throw RuntimeError("Heading line expected");
            }
            else if (reader.heading == "Added Associations") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    addedAssociations[reader.key]  = split(reader.value, ';');
                }
            }
            else if (reader.heading == "Removed Associations") {
                while (reader.getNextLine() == IniReader::KeyValue) {
                    removedAssociations[reader.key] = split(reader.value, ';');
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
    

    MimeappsListCollector::MimeappsListCollector()
    {
    }

    MimeappsListCollector::~MimeappsListCollector()
    {
    }

    void MimeappsListCollector::collect(const MimeappsList& mimeappsList)
    {
        for (auto pair : mimeappsList.addedAssociations) {
            for (string desktopId : pair.second) {
                addAssociation(pair.first, desktopId);  
            }
        }

        for (auto pair : mimeappsList.removedAssociations) {
            for (string desktopId : pair.second) {
                blacklistAssociation(pair.first, desktopId);
            }
        }

        for (auto pair : mimeappsList.defaultApps) {
            defaults[pair.first].insert(defaults[pair.first].end(), pair.second.begin(), pair.second.end());
        }

    }

    void MimeappsListCollector::addAssociation(std::string mimetype, std::string desktopId)
    {
        if (! contains(blacklist[mimetype], desktopId) && !contains(associations[mimetype], desktopId)) {
            associations[mimetype].push_back(desktopId);
        }
    }

    void MimeappsListCollector::blacklistAssociation(std::string mimetype, std::string desktopId)
    {
        if (! contains(blacklist[mimetype], desktopId)) {
            blacklist[mimetype].push_back(desktopId);
        }
    }

    bool MimeappsListCollector::contains(const AppList& applist, string desktopId)
    {
        return find(applist.begin(), applist.end(), desktopId) != applist.end();
    }

}