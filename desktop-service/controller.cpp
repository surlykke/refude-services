#include <set>
#include <ripc/service.h>
#include <ripc/localizedjsonresource.h>
#include <ripc/notifierresource.h>

#include "applicationcollector.h"
#include "mimetypecollector.h"

#include "controller.h"
#include "desktopwatcher.h"
namespace org_restfulipc
{

    Controller::Controller() : 
        service(),
        desktopWatcher(new DesktopWatcher(*this, true))        
    {
        service.map("/notify", std::make_shared<NotifierResource>());
        
        service.map("/applications", std::make_shared<LocalizedJsonResource>());
        ptr<LocalizedJsonResource>("/applications")->setJson(JsonConst::EmptyObject);
        
        service.map("/applications/filehandlers", std::make_shared<LocalizedJsonResource>());
        ptr<LocalizedJsonResource>("/applications/filehandlers")->setJson(JsonConst::EmptyObject);
        
        service.map("/applications/urlhandlers", std::make_shared<LocalizedJsonResource>());
        ptr<LocalizedJsonResource>("/applications/urlhandlers")->setJson(JsonConst::EmptyObject);

        service.map("/mimetypes", std::make_shared<LocalizedJsonResource>());
        ptr<LocalizedJsonResource>("/mimetypes")->setJson(JsonConst::EmptyObject);
    }

    Controller::~Controller()
    {
    }

    void Controller::setupAndRun()
    {
        desktopWatcher->start();
    }

    void Controller::update()
    {
        ApplicationCollector applicationCollector;
        applicationCollector.collect();
       
        MimetypeCollector mimetypeCollector;
        mimetypeCollector.collect();
       
        mimetypeCollector.addAssociations(applicationCollector.applicationJsons);
        mimetypeCollector.addDefaultApplications(applicationCollector.defaultApplications);

        updateApplicationHandersResources(applicationCollector.applicationJsons);
        updateResources(applicationCollector.applicationJsons , "/applications"); 
        updateResources(mimetypeCollector.mimetypesJson, "/mimetypes");
    }

    void Controller::updateApplicationHandersResources(Json& applicationJsons)
    {
        Json filehandlerJsons = JsonConst::EmptyObject;
        Json urlhandlerJsons = JsonConst::EmptyObject;
 
        applicationJsons.each([&](const char* applicationId, Json& applicationJson) {
            if (applicationJson.contains("Exec")) {
                if (strcasestr(applicationJson["Exec"], "%u")) {
                    filehandlerJsons[applicationId] = applicationJson.copy();
                    urlhandlerJsons[applicationId] = applicationJson.copy();
                }
                else if (strcasestr(applicationJson["Exec"], "%f")) {
                    filehandlerJsons[applicationId] = applicationJson.copy();
                }
            }
        });
        
        ptr<LocalizedJsonResource>("/applications/filehandlers")->setJson(std::move(filehandlerJsons));
        ptr<NotifierResource>("/notify")->resourceUpdated("applications/filehandlers");
        ptr<LocalizedJsonResource>("/applications/urlhandlers")->setJson(std::move(urlhandlerJsons));
        ptr<NotifierResource>("/notify")->resourceUpdated("applications/urlhandlers");
    }

    void Controller::updateResources(Json& jsons, std::string prefix)
    {
        Json& oldJson = ptr<LocalizedJsonResource>(prefix.data())->getJson();
       
        oldJson.each([&](const char* key, Json& json) {
            if (!jsons.contains(key)) {
                std::string path = prefix + "/" + key;
                service.unMap(path.data());
                ptr<NotifierResource>("/notify")->resourceRemoved(path.data() + 1);
            }
        });
        
        jsons.each([&, this](const char* key, Json& json) {
            if (oldJson.contains(key)) {
                if (oldJson[key] != json) {
                    std::string path = prefix + "/" + key;
                    ptr<LocalizedJsonResource>(path.data())->setJson(json.copy());
                    ptr<NotifierResource>("/notify")->resourceUpdated(path.data() + 1);
                }
            }
            else {
                auto resource = std::make_shared<LocalizedJsonResource>();
                resource->setJson(json.copy());
                std::string path = prefix + "/" + key;
                service.map(path.data(), resource);
                ptr<NotifierResource>("/notify")->resourceAdded(path.data() + 1);
            }

        });

        ptr<LocalizedJsonResource>(prefix.data())->setJson(std::move(jsons));
        ptr<NotifierResource>("/notify")->resourceUpdated(prefix.data() + 1);
    }

    void Controller::updateMimetypesResources(Json& newMimetypes)
    {
        
    }

}
