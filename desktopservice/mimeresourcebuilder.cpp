#include <tinyxml2.h>
#include "jsonresource.h"
#include "rootTemplate.h"
#include "typeTemplate.h"
#include "subtypeTemplate.h"
#include "mimeresourcebuilder.h"

using namespace tinyxml2;
namespace org_restfulipc 
{
    MimeResourceBuilder::MimeResourceBuilder(Service* service) :
        service(service)
    {
    }

    MimeResourceBuilder::~MimeResourceBuilder()
    {
    }

    void MimeResourceBuilder::build(const char* xmlFilePath, MimeAppMap& associations, MimeAppMap& defaults)
    {
        XMLDocument* doc = new XMLDocument;
        doc->LoadFile(xmlFilePath);
        XMLElement* rootElement = doc->FirstChildElement("mime-info");

        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        int i = 0;
        for (XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
             mimetypeElement;
             mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {
            
            char temp[128];
            char* saveptr;
            strncpy(temp, mimetypeElement->Attribute("type"), 127);
            string mimetype(temp); 
            char* typeName = strtok_r(temp, "/", &saveptr);
            char* subtypeName = strtok_r(NULL, "/", &saveptr);

            if (typeName == NULL || *typeName == '\0' || 
                subtypeName == NULL || *subtypeName == '\0') {
                std::cerr << "Warn: Invalid mimetype: " << mimetype << "\n";
            }

            Json subtypeJson = subtype(typeName, subtypeName);
            map<string, map<string, string> > translations;

            string commentTranslationKey = string("@@") + mimetype + "-comment";
            subtypeJson["comment"] = commentTranslationKey; 
            for (XMLElement* commentElement = mimetypeElement->FirstChildElement("comment");
                    commentElement;
                    commentElement = commentElement->NextSiblingElement("comment")) {
                string locale = commentElement->Attribute("xml:lang") ?  commentElement->Attribute("xml:lang") : ""; 
                transform(locale.begin(), locale.end(), locale.begin(), ::tolower);
                translations[locale][commentTranslationKey] = commentElement->GetText();
            }

            string acronymTranslationKey = string("@@") + mimetype + "-acronym";
            subtypeJson["acronym"] = acronymTranslationKey; 
            for (XMLElement* acronymElement = mimetypeElement->FirstChildElement("acronym");
                    acronymElement;
                    acronymElement = acronymElement->NextSiblingElement("acronym")) {
                string locale = acronymElement->Attribute("xml:lang") ?  acronymElement->Attribute("xml:lang") : "";
                transform(locale.begin(), locale.end(), locale.begin(), ::tolower);
                translations[locale][acronymTranslationKey] = acronymElement->GetText();
            }

            string expandedAcronymTranslationKey = string("@@") + mimetype + "-expanded-acronym";
            subtypeJson["expanded-acronym"] = expandedAcronymTranslationKey; 
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
                subtypeJson["aliases"].append(aliasElement->Attribute("type"));
            }

            for (XMLElement* globElement = mimetypeElement->FirstChildElement("glob");
                 globElement;
                 globElement = globElement->NextSiblingElement("glob")) {
                subtypeJson["globs"].append(globElement->Attribute("pattern"));
            }
           
            for (XMLElement* subclassOfElement = mimetypeElement->FirstChildElement("sub-class-of");
                 subclassOfElement;
                    subclassOfElement = subclassOfElement->NextSiblingElement("sub-class-of")) {
                subtypeJson["sub-class-of"].append(subclassOfElement->Attribute("type"));
            }

            XMLElement* iconElement = mimetypeElement->FirstChildElement("icon");
            if (iconElement) {
                subtypeJson["icon"] = iconElement->Attribute("name");
            }

            XMLElement* genericIconElement = mimetypeElement->FirstChildElement("generic-icon");
            if (genericIconElement) {
                subtypeJson["generic-icon"] = genericIconElement->Attribute("name");
            }

            for (string associatedApp : associations[mimetype]) {
                subtypeJson["associatedApplications"].append(associatedApp);
            }

            if (! defaults[mimetype].empty()) {
                subtypeJson["defaultApplication"] = defaults[mimetype][0];
            }

            LocalizedJsonResource* jsonResource = new LocalizedJsonResource();
            jsonResource->json = std::move(subtypeJson);
            jsonResource->translations = std::move(translations);
            service->map(jsonResource->json["_links"]["self"]["href"], jsonResource);

            i++;
        } 
    }


    Json& MimeResourceBuilder::root()
    {
        JsonResource* rootResource = (JsonResource*) service->mapping("/mimetypes");
        if (rootResource == NULL) {
            rootResource = new JsonResource();
            rootResource->json << rootTemplate_json;
            service->map("/mimetypes", rootResource);
            rootResource->setResponseStale();
        }
        return rootResource->json;
    }

    Json& MimeResourceBuilder::type(const char* typeName) 
    {
        char selfUri[128];
        snprintf(selfUri, 128, "/mimetypes/%s", typeName);
        JsonResource* typeResource = (JsonResource*) service->mapping(selfUri);
        if (typeResource == NULL)  {
            typeResource = new JsonResource();
            typeResource->json << typeTemplate_json;
            typeResource->json["name"] = typeName;
            typeResource->json["_links"]["self"]["href"] = selfUri;
            char subtypeRef[128];
            sprintf(subtypeRef, "/mimetypes/%s/{subtype}", typeName);
            typeResource->json["_links"]["subtype"]["href"] = subtypeRef;
            typeResource->setResponseStale();
            service->map(selfUri, typeResource);
            root()["types"].append(typeName);
        }
        return typeResource->json;
    }

    Json MimeResourceBuilder::subtype(const char* typeName, const char* subtype)
    {
        char selfUri[164];
        snprintf(selfUri, 164, "/mimetypes/%s/%s", typeName, subtype);
        type(typeName)["subtypes"].append(subtype);
        
        Json json;
        json << subtypeTemplate_json;
        json["type"] = typeName;
        json["subtype"] = subtype;
        json["_links"]["self"]["href"] = selfUri;
         
        return json;
    }



}