#include <tinyxml2.h>
#include <ripc/utils.h>

#include "subtypeTemplate.h"
#include "mimetypecollector.h"
namespace org_restfulipc
{

    MimetypeCollector::MimetypeCollector() : mimetypesJson(JsonConst::EmptyObject)
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
        tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument;
        doc->LoadFile("/usr/share/mime/packages/freedesktop.org.xml");
        tinyxml2::XMLElement* rootElement = doc->FirstChildElement("mime-info");


        if (!rootElement) throw RuntimeError("No 'mime-info' root-element");
        for (tinyxml2::XMLElement* mimetypeElement = rootElement->FirstChildElement("mime-type");
            mimetypeElement;
            mimetypeElement = mimetypeElement->NextSiblingElement("mime-type")) {

            std::string mimetype = mimetypeElement->Attribute("type");
            std::vector<std::string> tmp = split(mimetype, '/');
            if (tmp.size() != 2 || tmp[0].empty() || tmp[1].empty()) {
                std::cerr << "Incomprehensible mimetype: " << mimetype;
                continue;
            }
            std::string& typeName = tmp[0];
            std::string& subtypeName = tmp[1];
            Json& json = mimetypesJson[mimetype];
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
                json["icon"] = typeName + '-' + subtypeName;
            }

            tinyxml2::XMLElement* genericIconElement = mimetypeElement->FirstChildElement("generic-icon");
            if (genericIconElement) {
                json["genericIcon"] = genericIconElement->Attribute("name");
            }
            else {
                json["genericIcon"] = typeName + "-x-generic";
            }
        }
    }

    void MimetypeCollector::addAssociations(Json& applications)
    {
        applications.each([this](const char* desktopId, Json& desktopJson) {
            Json& mimetypes = desktopJson["MimeType"];
            for (int i = 0; i < mimetypes.size(); i++) {
                std::string mimetype = (const char*)mimetypes[i];
                if (mimetypesJson.contains(mimetype)) {
                    mimetypesJson[mimetype]["associatedApplications"].append(desktopId);
                }
            }
        });

    }

    void MimetypeCollector::addDefaultApplications(Map<std::vector<std::string> >& defaultApplications)
    {
        defaultApplications.each([this](const char* mimetype, 
                                        std::vector<std::string>& defaultApplicationIds) {
            if (mimetypesJson.contains(mimetype)) {
                for (std::string& defaultApplicationId : defaultApplicationIds) {
                    mimetypesJson[mimetype]["defaultApplications"].append(defaultApplicationId);
                }
            }
        });

    }

}