#include <unistd.h>
#include <climits>
#include <png++/png.hpp>
#include <refude/utils.h>

#include "runningappsicons.h"
#include "xdg.h"

namespace refude
{
    static const std::string icondirRoot = xdg::cache_home() + "/RefudeService/icons";

    RunningAppsIcons::RunningAppsIcons() : WebServer(icondirRoot.data())
    {
        for (const std::string& subdirectory : subdirectories(icondirRoot)) {
            unsigned long size = atol(subdirectory.data());
            if (size <= 0) {
                continue;
            }
            std::string subdiretoryPath = icondirRoot + "/" + subdirectory;
            for (const std::string& filename : files(subdiretoryPath,{".png"})) {
                std::string iconName = filename.substr(0, filename.size() - 4);
                std::string relativeIconPath = subdirectory + "/" + filename;
                iconPaths[iconName.data()].push_back({size, relativeIconPath});
            }
        }
    }

    RunningAppsIcons::~RunningAppsIcons()
    {
    }

    PathMimetypePair RunningAppsIcons::findFile(HttpMessage& request)
    {
        const char* iconName = request.remainingPath;

        if (! iconPaths.contains(iconName)) {
            std::cout << "name ikke fundet\n";
            throw HttpCode::Http404;
        }

        int size = 32;
        const char* sizeS = request.parameter("size");
        if (sizeS) {
            std::cout << "Looking for size: " << sizeS << "\n";
            char* endptr;
            size = strtoul(request.queryParameterMap["size"][0], &endptr, 10);
            if (*endptr) {
                throw HttpCode::Http406;
            } 
        }

        unsigned long distance = ULONG_MAX;
        const char* path ;
        for (const SizePath& sizePath : iconPaths[iconName]) {
            unsigned long newdist = sizePath.size > size ? sizePath.size - size : size - sizePath.size;
            if (newdist < distance) {
                distance = newdist;
                path = sizePath.path.data();
            }
        }
        std::cout << "RunningAppsIcons, returning: " << path << "\n";
        return {path, "image/png"};
    }


    void RunningAppsIcons::addIcon(const char* iconName, const long* icon, int nitems)
    {
        for (int pos = 0; pos < nitems; pos = pos + 2 + icon[pos] * icon[pos + 1]) {
            if (icon[pos] != icon[pos + 1]) {
                std::cerr << "Icon not square..\n";
                continue;
            }
           
            unsigned long size = icon[pos];
            std::string dirName = std::to_string(size);
            
            if (access((icondirRoot + "/" + dirName + "/" + iconName + ".png").data(), F_OK) == -1) {
                if (access((icondirRoot + "/" + dirName).data(), F_OK) == -1) {
                    std::string mkdirCmd = std::string("mkdir -p ") + icondirRoot + "/" + dirName;
                    system(mkdirCmd.data());
                }
                writePng((icondirRoot + "/" + dirName + "/" + iconName + ".png").data(), icon, pos, nitems);
                iconPaths[iconName].push_back({size, dirName + "/" + iconName + ".png"});
            }
        }

    }

    void RunningAppsIcons::writePng(const char* filePath, const long* data, unsigned long pos, unsigned long bound)
    {
        unsigned long width = data[pos++];
        unsigned long height = data[pos++];
        
        std::cout << "Create png of size " << height << ", " << width << "\n";
        png::image<png::rgba_pixel> img(height, width);
        for (unsigned long row = 0; row < height; row++) {
            for (unsigned long column = 0; column < width; column++) {
                if (pos + row*width + column > bound - 1) {
                    break;
                }
                unsigned long pxl = data[pos + row*width + column];
                png::byte A = (pxl & 0xFF000000) >> 24;
                png::byte R = (pxl & 0xFF0000) >> 16;
                png::byte G = (pxl & 0xFF00) >> 8;
                png::byte B = (pxl & 0xFF);
                img[row][column] = png::rgba_pixel(R, G, B, A);
            }
        }
        std::cout << "Writing: " << filePath << "\n";
        img.write(filePath);
    }



}
