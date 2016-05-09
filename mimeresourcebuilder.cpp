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
using namespace tinyxml2;
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

    void handleLocalizedXmlElement(XMLElement* mimetypeElement, const char* elementName, Json& json)
    {
        json[elementName] = JsonConst::EmptyObject;
        json[elementName]["_ripc:localized"] = "";
        for (XMLElement* element = mimetypeElement->FirstChildElement(elementName);
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
        XMLDocument* doc = new XMLDocument;
        doc->LoadFile("/usr/share/mime/packages/freedesktop.org.xml");
        XMLElement* rootElement = doc->FirstChildElement("mime-info");


        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        for (XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
            mimetypeElement;
            mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            string mimetype = mimetypeElement->Attribute("type");
            vector<string> tmp = split(mimetype, '/');
            if (tmp.size() != 2 || tmp[0].empty() || tmp[1].empty()) {
                std::cerr << "Incomprehensible mimetype: " << mimetype;
                continue;
            }
            string& typeName = tmp[0];
            string& subtypeName = tmp[1];
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

            for (XMLElement* aliasElement = mimetypeElement->FirstChildElement("alias");
                aliasElement;
                aliasElement = aliasElement->NextSiblingElement("alias")) {
                json["aliases"].append(aliasElement->Attribute("type"));
            }

            for (XMLElement* globElement = mimetypeElement->FirstChildElement("glob");
                globElement;
                globElement = globElement->NextSiblingElement("glob")) {
                json["globs"].append(globElement->Attribute("pattern"));
            }

            for (XMLElement* subclassOfElement = mimetypeElement->FirstChildElement("sub-class-of");
                subclassOfElement;
                subclassOfElement = subclassOfElement->NextSiblingElement("sub-class-of")) {
                json["subclassOf"].append(subclassOfElement->Attribute("type"));
            }

            XMLElement* iconElement = mimetypeElement->FirstChildElement("icon");
            if (iconElement) {
                json["icon"] = iconElement->Attribute("name");
            }
            else {
                json["icon"] = typeName + '-' + subtypeName;
            }

            XMLElement* genericIconElement = mimetypeElement->FirstChildElement("generic-icon");
            if (genericIconElement) {
                json["genericIcon"] = genericIconElement->Attribute("name");
            }
            else {
                json["genericIcon"] = typeName + "-x-generic";
            }
        }
    }

    void MimeResourceBuilder::addAssociationsAndDefaults(Map<Json>& desktopJsons, Map<vector<string>>&defaultApplications)
    {
        desktopJsons.each([this](const char* desktopId, Json& desktopJson) {
            Json& mimetypes = desktopJson["MimeType"];
            for (int i = 0; i < mimetypes.size(); i++) {
                string mimetype = (const char*)mimetypes[i];
                if (mimetypeJsons.contains(mimetype)) {
                    mimetypeJsons[mimetype]["associatedApplications"].append(desktopId);
                }
            }
        });

        defaultApplications.each([this](const char* mimetype, vector<string>& defaultApplicationIds) {
            if (mimetypeJsons.contains(mimetype)) {
                for (string& defaultApplicationId : defaultApplicationIds) {
                    mimetypeJsons[mimetype]["defaultApplications"].append(defaultApplicationId);
                }
            }
        });
    }

    void MimeResourceBuilder::mapResources(Service& service, NotifierResource::ptr notifier)
    {
        MimetypeResource::ptr mimetypeResource =
            dynamic_pointer_cast<MimetypeResource>(service.mapping("/mimetypes", true));

        if (mimetypeResource) {
            mimetypeResource->setMimetypeJsons(move(mimetypeJsons), notifier);
        }
        else {
            mimetypeResource = make_shared<MimetypeResource>(move(mimetypeJsons));
            service.map("/mimetypes", mimetypeResource, true);
        }
    }
}