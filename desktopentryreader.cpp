#include <iostream>
#include "desktopentryreader.h"
#include "desktopTemplate.h"

namespace org_restfulipc 
{
    DesktopEntryReader::DesktopEntryReader() 
    {
        read();
    }

    DesktopEntryReader::~DesktopEntryReader() 
    {
    }

    void DesktopEntryReader::read() 
    {
        std::cout << desktopTemplate_json << "\n";
    }
}