#ifndef DESKTOPENTRYREADER_H
#define DESKTOPENTRYREADER_H
#include <fstream>
#include <string>
#include <list>

#include <ripc/json.h>
#include <ripc/jsonresource.h>
#include "inireader.h"

namespace org_restfulipc 
{
    using namespace std;
    class DesktopEntryReader : private IniReader
    {
    public:
        DesktopEntryReader(string desktopFilePath);
        virtual ~DesktopEntryReader();
        Json json;
        Translations translations;
    
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

