#ifndef DESKTOPENTRYREADER_H
#define DESKTOPENTRYREADER_H
#include <fstream>
#include <string>
#include <list>

#include <json.h>
#include "inireader.h"

namespace org_restfulipc 
{
    using namespace std;
    class DesktopEntryReader : private IniReader
    {
    public:
        DesktopEntryReader(std::string applicationsDir, std::string relativeFilePath);
        virtual ~DesktopEntryReader();
        string entryId;
        Json json;
        map<string, map<string, string> > translations;
        string currentTranslationKeyPrefix;
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

