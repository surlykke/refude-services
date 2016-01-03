#include <string.h>
#include <tinyxml2.h>
#include <json.h>
#include <jsonwriter.h>
#include <service.h>
#include <jsonresource.h>
#include <map.h>
#include "resourcebuilder.h"

using namespace tinyxml2;

namespace org_restfulipc
{

    ResourceBuilder::ResourceBuilder(const char* mimedir) :
        Service((uint16_t)7938),
        strings(list_create<char*>(256))
    {
        std::cout << "ResourceBuilder constructor, strings.size: " << strings->size << ", strings.capacity: " << strings->capacity << "\n";
        read(mimedir);
    }

    void ResourceBuilder::read(const char* xmlFilePath)
    {
        Map<AbstractResource*>*  resourceMap = map_create<AbstractResource*>(1024);

        XMLDocument* doc = new XMLDocument;
        std::cout << "loading: " << xmlFilePath << "\n";
        doc->LoadFile(xmlFilePath);
        XMLElement* rootElement = doc->FirstChildElement("mime-info");

        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        int i = 0;
        for (XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
             mimetypeElement;
             mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            JsonResource* subtypeResource = buildSubTypeResource(mimetypeElement->Attribute("type"));
            if (! subtypeResource) continue;

            XMLElement* commentElement = mimetypeElement->FirstChildElement("comment");
            if (commentElement && commentElement->GetText()) {
                subtypeResource->json["comment"] = list_append(strings, strdup(commentElement->GetText()));
            }

            for (XMLElement* aliasElement = mimetypeElement->FirstChildElement("alias");
                 aliasElement;
                 aliasElement = aliasElement->NextSiblingElement("alias")) {
                subtypeResource->json["aliases"].append(list_append(strings,strdup(aliasElement->Attribute("type"))));
            }

            for (XMLElement* globElement = mimetypeElement->FirstChildElement("glob");
                 globElement;
                 globElement = globElement->NextSiblingElement("glob")) {
                subtypeResource->json["globs"].append(list_append(strings, strdup(globElement->Attribute("pattern"))));
            }
            
            std::cout << "Out of loop\n";
            std::cout << "main: " << JsonWriter(&buildMimeTypesResource()->json).buffer.data << "\n";
            std::cout << "sub:  " << JsonWriter(&subtypeResource->json).buffer.data << "\n";
            i++;
        }

        std::cout << "Read done - " << i << " mimetypes\n";
    }


    JsonResource* ResourceBuilder::buildMimeTypesResource()
    {
        JsonResource* mimeTypesResource = (JsonResource*) mapping("/mimetypes");
        if (! mimeTypesResource) {
            mimeTypesResource = new JsonResource("/mimetypes");
            mimeTypesResource->json["types"] = JsonConst::EmptyArray;
            mimeTypesResource->json["_links"]["related"] = JsonConst::EmptyArray;
            mimeTypesResource->setResponseStale();
            map("/mimetypes", mimeTypesResource);
        }
        return mimeTypesResource;
    }

    JsonResource* ResourceBuilder::buildToplevelResource(const char* selfUriTmp)
    {
        std::cout << "Into buildToplevelResource, selfUri: " << selfUriTmp << "\n";
        JsonResource* toplevelResource = (JsonResource*) mapping(selfUriTmp);
        if (! toplevelResource) {
            const char* selfUri = list_append(strings, strdup(selfUriTmp));
            const char* typeName = selfUri + strlen("/mimetypes/");
            toplevelResource = new JsonResource(selfUri);
            toplevelResource->json["name"] = typeName;
            toplevelResource->json["subtypes"] = JsonConst::EmptyArray;
            toplevelResource->json["_links"]["related"] = JsonConst::EmptyObject;
            toplevelResource->json["_links"]["related"]["templated"] = JsonConst::TRUE;
            char relHref[128];
            sprintf(relHref, "%s/{subtype}", selfUri);
            toplevelResource->json["_links"]["related"]["href"] = list_append(strings, strdup(relHref));
            toplevelResource->setResponseStale();
            map(selfUri, toplevelResource);
            buildMimeTypesResource()->json["types"].append(typeName);
        }
    
        return toplevelResource;
    }

    JsonResource* ResourceBuilder::buildSubTypeResource(const char* typeString)
    {
        const char* slashPtr = strchr(typeString, '/');
        if (!slashPtr) return NULL;
        int slashIndex = slashPtr - typeString;
        if (slashIndex == 0 || slashIndex == strlen(typeString - 1)) return NULL;

        char tmp[256];
        sprintf(tmp, "/mimetypes/%s", typeString);
        const char* subtypeSelfUri = list_append(strings, strdup(tmp));
        const char* subtypeName = subtypeSelfUri + strlen("/mimetypes/") + slashIndex + 1;

        tmp[strlen("/mimetypes/") + slashIndex] = '\0';
        JsonResource* toplevelResource = buildToplevelResource(tmp);
        toplevelResource->json["subtypes"].append(subtypeName);
        JsonResource* subtypeResource = new JsonResource(subtypeSelfUri);
        subtypeResource->json["name"] = subtypeName;

        subtypeResource->json["aliases"] = JsonConst::EmptyArray;
        subtypeResource->json["globs"] = JsonConst::EmptyArray;
        subtypeResource->setResponseStale();
        map(subtypeSelfUri, subtypeResource);
        return subtypeResource;
    }

}

