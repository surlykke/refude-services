#ifndef RUNNINGAPPSICONS_H
#define RUNNINGAPPSICONS_H
#include <vector>
#include <string>
#include <utility>
#include <refude/map.h>
#include <refude/webserver.h>
namespace refude
{
    struct SizePath
    {
        unsigned long size;
        std::string path;
    };

    typedef std::vector<SizePath> SizePathList;

    class RunningAppsIcons : public WebServer
    {
    public:
        typedef std::shared_ptr<RunningAppsIcons> ptr;
        RunningAppsIcons();
        virtual ~RunningAppsIcons();
        virtual PathMimetypePair findFile(HttpMessage& request);
    
        void addIcon(const char* iconName, const long* icon, int elements);
    private:
        std::string rootDir; 
        Map<SizePathList> iconPaths;

        void writePng(const char* filePath, const long* data, unsigned long pos, unsigned long bound);
        
    };
}
#endif /* RUNNINGAPPSICONS_H */

