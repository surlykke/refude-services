/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */

#include <tinyxml2.h>
#include <ripc/jsonresource.h>
#include <ripc/utils.h>
#include <ripc/jsonwriter.h>

#include "rootTemplate.h"
#include "subtypeTemplate.h"
#include "mimeresourcebuilder.h"
#include "mimetyperesource.h"

namespace org_restfulipc
{

    MimeResourceBuilder::MimeResourceBuilder() :
        mimetypeJsons()
    {
    }

    MimeResourceBuilder::~MimeResourceBuilder()
    {
    }

    // Caller must ensure that mimetypeElement->FirstChildElement exists...

    void handleLocalizedXmlElement(tinyxml2::XMLElement* mimetypeElement, 
                                   const char* elementName, 
                                   Json& json)
    {
        json[elementName] = JsonConst::EmptyObject;
        json[elementName]["_ripc:localized"] = "";
        for (tinyxml2::XMLElement* element = mimetypeElement->FirstChildElement(elementName);
            element;
            element = element->NextSiblingElement(elementName)) {

            if (element->Attribute("xml:lang")) {
                json[elementName][element->Attribute("xml:lang")] = element->GetText();
            }
            else {
                json[elementName]["_ripc:localized"] = element->GetText();
            }
        }
    }

    void MimeResourceBuilder::build()
    {
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
        doc->LoadFile("/usr/share/mime/packages/freedesktop.org.xml");
        tinyxml2::XMLElement* rootElement = doc->FirstChildElement("mime-info");


        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        for (tinyxml2::XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
            mimetypeElement;
            mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            std::string mimetype = mimetypeElement->Attribute("type");
            std::vector<std::string> tmp = split(mimetype, '/');
            if (tmp.size() != 2 || tmp[0].empty() || tmp[1].empty()) {
                std::cerr << "Incomprehensible mimetype: " << mimetype;
                continue;
            }
            std::string& typeName = tmp[0];
            std::string& subtypeName = tmp[1];
            Json& json = mimetypeJsons[mimetype];
            json << subtypeTemplate_json;
            json["type"] = typeName;
            json["subtype"] = subtypeName;

            if (mimetypeElement->FirstChildElement("comment")) {
                handleLocalizedXmlElement(mimetypeElement, "comment", json);
            }

            if (mimetypeElement->FirstChildElement("acronym")) {
                handleLocalizedXmlElement(mimetypeElement, "acronym", json);
            }

            if (mimetypeElement->FirstChildElement("expanded-acronym")) {
                handleLocalizedXmlElement(mimetypeElement, "expanded-acronym", json);
            }

            for (tinyxml2::XMLElement* aliasElement = mimetypeElement->FirstChildElement("alias");
                aliasElement;
                aliasElement = aliasElement->NextSiblingElement("alias")) {
                json["aliases"].append(aliasElement->Attribute("type"));
            }

            for (tinyxml2::XMLElement* globElement = mimetypeElement->FirstChildElement("glob");
                globElement;
                globElement = globElement->NextSiblingElement("glob")) {
                json["globs"].append(globElement->Attribute("pattern"));
            }

            for (tinyxml2::XMLElement* subclassOfElement = mimetypeElement->FirstChildElement("sub-class-of");
                subclassOfElement;
                subclassOfElement = subclassOfElement->NextSiblingElement("sub-class-of")) {
                json["subclassOf"].append(subclassOfElement->Attribute("type"));
            }

            tinyxml2::XMLElement* iconElement = mimetypeElement->FirstChildElement("icon");
            if (iconElement) {
                json["icon"] = iconElement->Attribute("name");
            }
            else {
                json["icon"] = typeName + '-' + subtypeName;
            }

            tinyxml2::XMLElement* genericIconElement = mimetypeElement->FirstChildElement("generic-icon");
            if (genericIconElement) {
                json["genericIcon"] = genericIconElement->Attribute("name");
            }
            else {
                json["genericIcon"] = typeName + "-x-generic";
            }
        }
    }

    void MimeResourceBuilder::addAssociationsAndDefaults(Map<Json>& desktopJsons, 
                                                         Map<std::vector<std::string>>&defaultApplications)
    {
        desktopJsons.each([this](const char* desktopId, Json& desktopJson) {
            Json& mimetypes = desktopJson["MimeType"];
            for (int i = 0; i < mimetypes.size(); i++) {
                std::string mimetype = (const char*)mimetypes[i];
                if (mimetypeJsons.contains(mimetype)) {
                    mimetypeJsons[mimetype]["associatedApplications"].append(desktopId);
                }
            }
        });

        defaultApplications.each([this](const char* mimetype, 
                                        std::vector<std::string>& defaultApplicationIds) {
            if (mimetypeJsons.contains(mimetype)) {
                for (std::string& defaultApplicationId : defaultApplicationIds) {
                    mimetypeJsons[mimetype]["defaultApplications"].append(defaultApplicationId);
                }
            }
        });
    }
}