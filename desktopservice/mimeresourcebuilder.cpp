#include <tinyxml2.h>
#include "jsonresource.h"
#include "rootTemplate.h"
#include "subtypeTemplate.h"
#include "mimeresourcebuilder.h"
#include "mimetyperesource.h"
#include "jsonreader.h"
#include "utils.h"

using namespace tinyxml2;
namespace org_restfulipc 
{
    MimeResourceBuilder::MimeResourceBuilder() :
        rootResource(),
        mimetypeResources()
    {
        rootResource = make_shared<JsonResource>();
        build();
    }

    MimeResourceBuilder::~MimeResourceBuilder()
    {
    }

    void MimeResourceBuilder::build()
    {
        map<string, set<string> > mimetypes; 
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

            Json json;
            json << subtypeTemplate_json;
            json["type"] = typeName;
            json["subtype"] = subtypeName;
            Translations translations;

            string commentTranslationKey = string("@@") + mimetype + "-comment";
            json["comment"] = commentTranslationKey; 
            for (XMLElement* commentElement = mimetypeElement->FirstChildElement("comment");
                    commentElement;
                    commentElement = commentElement->NextSiblingElement("comment")) {
                string locale = commentElement->Attribute("xml:lang") ?  commentElement->Attribute("xml:lang") : ""; 
                transform(locale.begin(), locale.end(), locale.begin(), ::tolower);
                translations[locale][commentTranslationKey] = commentElement->GetText();
            }

            string acronymTranslationKey = string("@@") + mimetype + "_acronym";
            json["acronym"] = acronymTranslationKey; 
            for (XMLElement* acronymElement = mimetypeElement->FirstChildElement("acronym");
                    acronymElement;
                    acronymElement = acronymElement->NextSiblingElement("acronym")) {
                string locale = acronymElement->Attribute("xml:lang") ?  acronymElement->Attribute("xml:lang") : "";
                transform(locale.begin(), locale.end(), locale.begin(), ::tolower);
                translations[locale][acronymTranslationKey] = acronymElement->GetText();
            }

            string expandedAcronymTranslationKey = string("@@") + mimetype + "_expandedAcronym";
            json["expandedAcronym"] = expandedAcronymTranslationKey; 
            for (XMLElement* expandedAcronymElement = mimetypeElement->FirstChildElement("expanded-acronym");
                    expandedAcronymElement;
                    expandedAcronymElement = expandedAcronymElement->NextSiblingElement("expanded-acronym")) {
                string locale = expandedAcronymElement->Attribute("xml:lang") ?  expandedAcronymElement->Attribute("xml:lang") : ""; 
                transform(locale.begin(), locale.end(), locale.begin(), ::tolower);
                translations[locale][expandedAcronymTranslationKey] = expandedAcronymElement->GetText();
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

            XMLElement* genericIconElement = mimetypeElement->FirstChildElement("generic-icon");
            if (genericIconElement) {
                json["genericIcon"] = genericIconElement->Attribute("name");
            }

            mimetypeResources[mimetype] = make_shared<MimetypeResource>();
            mimetypeResources[mimetype]->setJson(std::move(json), std::move(translations));

            mimetypes[typeName].insert(subtypeName);
        } 

        Json rootJson; 
        rootJson << rootTemplate_json;
        for (const auto& it : mimetypes) {
            Json& subtypeArray = rootJson["mimetypes"][it.first] = JsonConst::EmptyArray;
            for (const string& subtype : it.second) {
                subtypeArray.append(subtype);
            }
        }

        rootResource->setJson(std::move(rootJson));
    }


}