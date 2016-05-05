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
        json[elementName]["_ripc:localized"] = JsonConst::TRUE;
        for (XMLElement* element = mimetypeElement->FirstChildElement(elementName);
                element;
                element = element->NextSiblingElement(elementName)) {
            
            string locale = element->Attribute("xml:lang") ?  element->Attribute("xml:lang") : ""; 
            json[elementName][locale] = element->GetText();
            json["_ripc:locales"][locale] = "";
        }
    }

    void MimeResourceBuilder::build()
    {
        std::cout << "MimeResourceBuilder::build()...\n";
        
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

    void MimeResourceBuilder::addAssociationsAndDefaults(const AppSets& associations, const AppLists& defaults)
    {
        for (auto& p : associations) {
            if (mimetypeJsons.contains(p.first)) {
                for (const string& entryId : p.second) {
                    mimetypeJsons[p.first]["associatedApplications"].append(entryId);
                }
            }
        }

        for (const auto& it : defaults) {
            if (it.second.size() > 0) {
                if (mimetypeJsons.contains(it.first)) {
                    mimetypeJsons[it.first]["defaultApplication"] = it.second[0];
                }
            }
        }
    }

    void MimeResourceBuilder::mapResources(Service& service, NotifierResource::ptr notifier)
    {
        std::cout << "MimeResourceBuilder::mapResources()...\n";

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