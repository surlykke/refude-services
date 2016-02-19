#ifndef DESKTOPENTRYREADER_H
#define DESKTOPENTRYREADER_H
#include <fstream>
#include <string>
#include <list>

#include <json.h>
#include "inireader.h"

namespace org_restfulipc 
{
    class DesktopEntryReader : private IniReader
    {
    public:
        DesktopEntryReader(std::string applicationsDir, std::string relativeFilePath);
        virtual ~DesktopEntryReader();
        std::string entryId;
        Json json;
    private:
        void read();
        void readMainGroup();
        void readGroup();

        void readKeyValues(Json& json);
        bool readKeyValue(Json& json);
        void handleBool(Json&, std::string value);
        std::list<std::string> toList(std::string value);

        bool keyOneOf(std::list<std::string> list);
    };
}

#endif /* DESKTOPENTRYREADER_H */

