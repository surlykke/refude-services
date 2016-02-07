#include <string.h>
#include <tinyxml2.h>
#include <json.h>
#include <jsonwriter.h>
#include <service.h>
#include <jsonresource.h>
#include <map.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "resourcebuilder.h"
#include "rootTemplate.h"
#include "typeTemplate.h"
#include "subtypeTemplate.h"

using namespace tinyxml2;

namespace org_restfulipc
{

    ResourceBuilder::ResourceBuilder(const char* mimedir) :
        Service((uint16_t)7938)
    {
        readXml(mimedir);
    }

    ResourceBuilder::~ResourceBuilder() {
    }

    void ResourceBuilder::readXml(const char* xmlFilePath)
    {
        XMLDocument* doc = new XMLDocument;
        doc->LoadFile(xmlFilePath);
        XMLElement* rootElement = doc->FirstChildElement("mime-info");

        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        int i = 0;
        for (XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
             mimetypeElement;
             mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            char mimetype[128];
            char* saveptr;
            strncpy(mimetype, mimetypeElement->Attribute("type"), 127);
            char* typeName = strtok_r(mimetype, "/", &saveptr);
            char* subtypeName = strtok_r(NULL, "/", &saveptr);

            if (typeName == NULL || *typeName == '\0' || 
                subtypeName == NULL || *subtypeName == '\0') {
                std::cerr << "Warn: Invalid mimetype: " << mimetype << "\n";
            }

            Json& subtypeJson = subtype(typeName, subtypeName);

            XMLElement* commentElement = mimetypeElement->FirstChildElement("comment");
            if (commentElement && commentElement->GetText()) {
                subtypeJson["comment"] = commentElement->GetText();
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
            
            i++;
        } 
    }


    Json& ResourceBuilder::root()
    {
        JsonResource* rootResource = (JsonResource*) mapping("/mimetypes");
        if (rootResource == NULL) {
            rootResource = new JsonResource();
            rootResource->json << rootTemplate_json;
            map("/mimetypes", rootResource);
            rootResource->setResponseStale();
        }
        return rootResource->json;
    }

    Json& ResourceBuilder::type(const char* typeName) 
    {
        char selfUri[128];
        snprintf(selfUri, 128, "/mimetypes/%s", typeName);
        JsonResource* typeResource = (JsonResource*) mapping(selfUri);
        if (typeResource == NULL)  {
            typeResource = new JsonResource();
            typeResource->json << typeTemplate_json;
            typeResource->json["name"] = typeName;
            typeResource->json["_links"]["self"]["href"] = selfUri;
            char subtypeRef[128];
            sprintf(subtypeRef, "/mimetypes/%s/{subtype}", typeName);
            typeResource->json["_links"]["subtype"]["href"] = subtypeRef;
            typeResource->setResponseStale();
            map(selfUri, typeResource);
            root()["types"].append(typeName);
        }
        return typeResource->json;
    }

    Json& ResourceBuilder::subtype(const char* typeName, const char* subtype)
    {
        char selfUri[164];
        snprintf(selfUri, 164, "/mimetypes/%s/%s", typeName, subtype);
        JsonResource* subtypeResource = new JsonResource(); 
        subtypeResource->json << subtypeTemplate_json;
        subtypeResource->json["type"] = typeName;
        subtypeResource->json["subtype"] = subtype;
        subtypeResource->json["_links"]["self"]["href"] = selfUri;
        subtypeResource->setResponseStale();
        map(selfUri, subtypeResource);
        type(typeName)["subtypes"].append(subtype);
        
        return subtypeResource->json;
    }
}

