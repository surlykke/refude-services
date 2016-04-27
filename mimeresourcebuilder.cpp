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
        root(), 
        jsons()
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
        root << rootTemplate_json;
        
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
            string typeName = tmp[0];
            string subtypeName = tmp[1];
            string url = string("/mimetype/") + mimetype;
            Json& json = jsons[url];
            json << subtypeTemplate_json;
            json["type"] = typeName;
            json["subtype"] = subtypeName;
            json["_links"]["self"]["href"] = url;

            if (root["mimetypes"][typeName].undefined()) {
                root["mimetypes"][typeName] = JsonConst::EmptyArray;
            }
            root["mimetypes"][typeName].append(subtypeName);
 
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
            string url = string("/mimetype/") + p.first;
            if (jsons.find(url.data()) > 0) {
                for (const string& entryId : p.second) {
                    jsons[url]["associatedApplications"].append(entryId);
                }
            }
        }

        for (const auto& it : defaults) {
            if (it.second.size() > 0) {
                string url = "/mimetype/" + it.first;
                if (jsons.find(url.data()) >= 0) {
                    jsons[url]["defaultApplication"] = it.second[0];
                }
            }
        }
    }

    void MimeResourceBuilder::mapResources(Service& service, NotifierResource::ptr notifier)
    {
        for (const char* url : service.resourceMappings.keys("/mimetype/")) {
            if (jsons.find(url) < 0) {
                const char* mimetype = url + strlen("/mimetype/");
                service.unMap(url);
                notifier->notifyClients("mimetype-removed", mimetype);
            }
        }

        for (const char* url : jsons.keys()) {
            const char* mimetype = url + strlen("/mimetype/");
            MimetypeResource::ptr res = dynamic_pointer_cast<MimetypeResource>(service.mapping(url));
            if (res) {
                if (res->getJson() != jsons[url]) {
                    res->setJson(move(jsons[url]));
                    notifier->notifyClients("mimetype-updated", mimetype);
                }
            }
            else {
                res = make_shared<MimetypeResource>();
                res->setJson(move(jsons[url]));
                service.map(url, res);
                notifier->notifyClients("mimetype-added", mimetype);
            }
        }

        JsonResource::ptr rootResource = dynamic_pointer_cast<JsonResource>(service.mapping("/mimetypes"));
        if (rootResource) {
            if (rootResource->getJson() != root) {
                rootResource->setJson(move(root));
                notifier->notifyClients("mimetypelist-updated", "");
            }
        }
        else {
            rootResource = make_shared<JsonResource>();
            rootResource->setJson(move(root));
            service.map("/mimetypes", rootResource);
            notifier->notifyClients("mimetypelist-added", "");
        }
    }

}