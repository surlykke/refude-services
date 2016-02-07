#ifndef DESKTOPENTRYREADER_H
#define DESKTOPENTRYREADER_H

namespace org_restfulipc 
{
    class DesktopEntryReader
    {
    public:
        DesktopEntryReader();
        virtual ~DesktopEntryReader();
    private:
        void read();
    };
}

#endif /* DESKTOPENTRYREADER_H */

