#ifndef DESKTOPENTRYREADER_H
#define DESKTOPENTRYREADER_H
#include <json.h>
namespace org_restfulipc 
{
    class DesktopEntryReader
    {
    public:
        DesktopEntryReader();
        virtual ~DesktopEntryReader();
    private:
        void read(const char* desktopFilePath);
        void handleGroupHeading(std::string heading);
        void handleKeyValuePair(std::string key, std::string locale, std::string value);
        void handleStringValue(std::string key, std::string value);
        void handleBool(std::string key, std::string value);
        void handleStringlistKey(std::string key, std::string value);

        Json json;
        int state;
        std::string currentAction;
    };
}

#endif /* DESKTOPENTRYREADER_H */

