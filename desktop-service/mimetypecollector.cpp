/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <tinyxml2.h>
#include "utils.h"

#include "mimetypecollector.h"
namespace refude
{

    MimetypeCollector::MimetypeCollector() : 
        jsonArray(JsonConst::EmptyArray)
    {
    }

    MimetypeCollector::~MimetypeCollector()
    {
    }


    void handleLocalizedXmlElement(tinyxml2::XMLElement* mimetypeElement, 
                                   const char* elementName, 
                                   Json& json)
    {
        std::string _ripc_localized_elementName = std::string("_ripc:localized:") + elementName;
        json[_ripc_localized_elementName] = JsonConst::EmptyObject;
        for (tinyxml2::XMLElement* element = mimetypeElement->FirstChildElement(elementName);
            element;
            element = element->NextSiblingElement(elementName)) {

            if (element->Attribute("xml:lang")) {
                json[_ripc_localized_elementName][element->Attribute("xml:lang")] = element->GetText();
            }
            else {
                json[_ripc_localized_elementName][""] = element->GetText();
            }
        }
    }

    void MimetypeCollector::collect()
    {
        const char* mimetypeTemplate = R"json(
        {
            "globs" : [],
            "aliases" : [],
            "subclassOf" : [],
            "defaultApplications" : [],
            "associatedApplications" : []
        }              
        )json";


        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
        doc->LoadFile("/usr/share/mime/packages/freedesktop.org.xml");
        tinyxml2::XMLElement* rootElement = doc->FirstChildElement("mime-info");


        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        for (tinyxml2::XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
            mimetypeElement;
            mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            std::string mimetype = mimetypeElement->Attribute("type");
            Json json;
            json << mimetypeTemplate;
            json["mimetype"] = mimetype;
            
            std::vector<std::string> tmp = split(mimetype, '/');
            if (tmp.size() != 2 || tmp[0].empty() || tmp[1].empty()) {
                std::cerr << "Incomprehensible mimetype: " << mimetype;
                continue;
            }
            
            std::string& type = tmp[0];
            json["type"] = type;
            
            std::string& subtype = tmp[1];
            json["subtype"] = subtype;

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
                json["icon"] = type + '-' + subtype;
            }

            tinyxml2::XMLElement* genericIconElement = mimetypeElement->FirstChildElement("generic-icon");
            if (genericIconElement) {
                json["genericIcon"] = genericIconElement->Attribute("name");
            }
            else {
                json["genericIcon"] = type + "-x-generic";
            }
            
            jsonArray.append(std::move(json));
        }

    }

    void MimetypeCollector::addAssociations(Json& applicationArray)
    {
        Map<uint> index;
        for (uint i = 0; i < jsonArray.size(); i++) {
            index[jsonArray[i]["mimetype"].toString()] = i;
        }


        applicationArray.eachElement([this, &index](Json& appJson) {
            const char* appId = appJson["applicationId"].toString();
            appJson["mimetype"].eachElement([this, &index, &appId](const Json& mimetype) {
                int pos = index.find(mimetype.toString());
                if (pos >= 0) {
                    jsonArray[index.pairAt(pos).value]["associatedApplications"].append(appId);
                }
            });
        });

    }

    void MimetypeCollector::addDefaultApplications(Map<std::vector<std::string> >& defaultApplications)
    {
        Map<uint> index;
        for (uint i = 0; i < jsonArray.size(); i++) {
            index[jsonArray[i]["mimetype"].toString()] = i;
        }

        defaultApplications.each([this, &index](const char* mimetype, std::vector<std::string>& defaultApplicationIds) {
            int pos = index.find(mimetype);
            if (pos >= 0) {
                for (std::string& defaultApplicationId : defaultApplicationIds) {
                    jsonArray[index.pairAt(pos).value]["defaultApplications"].append(defaultApplicationId);
                }
            }
        });

    }

}
