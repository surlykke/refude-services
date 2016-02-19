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
            
            for (string associatedApp : associations[mimetype]) {
                subtypeJson["associatedApplications"].append(associatedApp);
            }

            if (! defaults[mimetype].empty()) {
                subtypeJson["defaultApplication"] = defaults[mimetype][0];
            }

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

    Json& MimeResourceBuilder::subtype(const char* typeName, const char* subtype)
    {
        char selfUri[164];
        snprintf(selfUri, 164, "/mimetypes/%s/%s", typeName, subtype);
        JsonResource* subtypeResource = new JsonResource(); 
        subtypeResource->json << subtypeTemplate_json;
        subtypeResource->json["type"] = typeName;
        subtypeResource->json["subtype"] = subtype;
        subtypeResource->json["_links"]["self"]["href"] = selfUri;
        subtypeResource->setResponseStale();
        service->map(selfUri, subtypeResource);
        type(typeName)["subtypes"].append(subtype);
        
        return subtypeResource->json;
    }



}