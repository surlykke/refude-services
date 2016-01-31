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

using namespace tinyxml2;

namespace org_restfulipc
{

    char* readFile(const char* path) 
    {
        int fd = open(path, O_RDONLY);
        if (fd < 0) throw C_Error();
        
        struct stat st;
        if (fstat(fd, &st) < 0) throw C_Error();
        
        char* result = new char[st.st_size + 1];
        for (int bytesRead = 0; bytesRead < st.st_size; ) {
            int n = read(fd, result, st.st_size - bytesRead);
            if (n < 0) throw C_Error();
            bytesRead += n;
        }
        result[st.st_size] = '\0';

        return result; 
    }

    ResourceBuilder::ResourceBuilder(const char* mimedir) :
        Service((uint16_t)7938)    
    {
        read(mimedir);
        JsonResource* rootResource = new JsonResource();
        rootResource->json = std::move(root);
        rootResource->setResponseStale();
        map("/mimetypes", rootResource);
    }

    void ResourceBuilder::read(const char* xmlFilePath)
    {
        buildRoot();
        std::cout << JsonWriter(&root).buffer.data << "\n";
        XMLDocument* doc = new XMLDocument;
        doc->LoadFile(xmlFilePath);
        XMLElement* rootElement = doc->FirstChildElement("mime-info");

        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        int i = 0;
        for (XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
             mimetypeElement;
             mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            char mimetype[128];
            if (sprintf(mimetype, mimetypeElement->Attribute("type"), 128) > 127)  {
                std::cout << "Warn: mimetype too long: " << mimetypeElement->Attribute("type");
                return;
            }

            char* typeName = mimetype;
            char* subtype = NULL;
            for (char *c = mimetype; *c; c++) {
                if ('/' == *c) {
                    *c = '\0';
                    subtype = c + 1;
                }
            }

            if (*typeName == '\0' || subtype == NULL || *subtype == '\0') {
                std::cerr << "Warn: Invalid mimetype: " << mimetypeElement->Attribute("type");
            }

            Json& subtypeJson = buildSubtype(typeName, subtype);

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


    void ResourceBuilder::buildRoot()
    {
        const char* jsonTemplate = readFile("rootTemplate.json");
        root << jsonTemplate;
        delete jsonTemplate;
    }

    Json& ResourceBuilder::buildType(const char* typeName) 
    {
        static char* typeTemplate = readFile("typeTemplate.json");

        char selfUri[128];
        snprintf(selfUri, 128, "/mimetypes/%s", typeName);
        
        Json& typeJson = root["ripc::prefetched"][selfUri];
        if (typeJson.mType == JsonType::Undefined) {
            typeJson << typeTemplate;
            typeJson["name"] = typeName;
            typeJson["_links"]["self"]["href"] = selfUri;
            char relatedHref[32];
            sprintf(relatedHref, "/mimetypes/%s/{subtype}", typeName);
            typeJson["_links"]["related"][0]["href"] = relatedHref;
            
        }
        return typeJson;
    }

    Json& ResourceBuilder::buildSubtype(const char* typeName, const char* subtype)
    {
        static char* subtypeTemplate = readFile("subtypeTemplate.json");

        char selfUri[164];
        snprintf(selfUri, 164, "/mimetypes/%s/%s", typeName, subtype);
        Json& typeJson = buildType(typeName);
        Json& subtypeJson = typeJson["ripc::prefetched"][selfUri];
        subtypeJson << subtypeTemplate;
        subtypeJson["type"] = typeName;
        subtypeJson["subtype"] = subtype;
        subtypeJson["_links"]["self"]["href"] = selfUri;
        return subtypeJson;
    }
}

