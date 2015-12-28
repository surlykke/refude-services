#include <string.h>
#include <tinyxml2.h>
#include <json.h>
#include <jsonwriter.h>

#include "resourcebuilder.h"

using namespace tinyxml2;
ResourceBuilder::ResourceBuilder(const char* mimedir)
{
    stringRepo = (char*) malloc(1024);
    if (!stringRepo) throw C_Error();
    stringRepoUsed = 0;
    stringRepoCapacity = 1024;
    read(mimedir);
}

void ResourceBuilder::read(const char* xmlFilePath)
{
    XMLDocument* doc = new XMLDocument;
    std::cout << "loading: " << xmlFilePath << "\n";
    doc->LoadFile(xmlFilePath);
    XMLElement* rootElement = doc->FirstChildElement("mime-info");

    if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
    int i = 0;
    for (XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
         mimetypeElement;
         mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

        Json mt = JsonConst::EmptyObject;

        char* type = dup(mimetypeElement->Attribute("type"));
        char* subType = 0;
        for (char* c = type; *c; c++) {
            if ('/' == *c) {
                subType = c;
                *subType++ = '\0';
            }
        }
        mt["type"] = type;
        mt["subType"] = subType;

        XMLElement* commentElement = mimetypeElement->FirstChildElement("comment");
        if (commentElement && commentElement->GetText()) {
            mt["comment"] = dup(commentElement->GetText());
        }
        else {
            std::cout << "Warning: No comment on mimetype\n";
            continue;
        }


        Json aliases = JsonConst::EmptyArray;
        for (XMLElement* aliasElement = mimetypeElement->FirstChildElement("alias");
             aliasElement;
             aliasElement = aliasElement->NextSiblingElement("alias")) {
            aliases.append(dup(aliasElement->Attribute("type")));
        }
        mt["aliases"] = std::move(aliases);

        Json globs = JsonConst::EmptyArray;
        for (XMLElement* globElement = mimetypeElement->FirstChildElement("glob");
             globElement;
             globElement = globElement->NextSiblingElement("glob")) {
            globs.append(dup(globElement->Attribute("pattern")));
        }
        mt["globs"] = std::move(globs);

        std::cout << JsonWriter(&mt).buffer.data;
        std::cout << "\n";
        i++;
    }

    std::cout << "Read done - " << i << " mimetypes\n";
}

char* ResourceBuilder::dup(const char* str)
{
    if (!str) return NULL;
    int len = strlen(str);
    while (stringRepoUsed + len + 1 > stringRepoCapacity)  {
        stringRepoCapacity = stringRepoCapacity*2;
        stringRepo = (char*) realloc(stringRepo, stringRepoCapacity);
        if (!stringRepo) throw C_Error();
    }

    char* newString = strcpy(stringRepo + stringRepoUsed, str);
    stringRepoUsed = stringRepoUsed + len + 1;
    return newString;
}


