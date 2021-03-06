/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <unistd.h>
#include <climits>
#include <png++/png.hpp>
#include "utils.h"
#include "comm.h"

#include "runningappsicons.h"
#include "xdg.h"

namespace refude
{
    static const std::string icondirRoot = xdg::cache_home() + "/RefudeService/icons";

    RunningAppsIcons::RunningAppsIcons() : AbstractResource()
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

    void RunningAppsIcons::doGET(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {

        if (iconPaths.find(remainingPath) < 0) {
            return sendStatus(socket, HttpCode::Http404);
        }

        int size = 32;
        const char* sizeS = request.parameter("size");
        if (sizeS) {
            char* endptr;
            size = strtoul(request.queryParameterMap["size"][0], &endptr, 10);
            if (*endptr) {
                return sendStatus(socket, HttpCode::Http406);
            } 
        }

        unsigned long distance = ULONG_MAX;
        const char* path ;
        for (const SizePath& sizePath : iconPaths[remainingPath]) {
            unsigned long newdist = sizePath.size > size ? sizePath.size - size : size - sizePath.size;
            if (newdist < distance) {
                distance = newdist;
                path = sizePath.path.data();
            }
        }
        return sendFile(socket, path, "image/png");
    }


    void RunningAppsIcons::addIcon(const char* iconName, const std::vector<unsigned long>& icon)
    {
        for (unsigned long pos = 0; pos < icon.size(); pos = pos + 2 + icon[pos] * icon[pos + 1]) {
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
                writePng((icondirRoot + "/" + dirName + "/" + iconName + ".png").data(), icon, pos);
                iconPaths[iconName].push_back({size, dirName + "/" + iconName + ".png"});
            }
        }

    }

    void RunningAppsIcons::writePng(const char* filePath, const std::vector<unsigned long>& data, unsigned long pos)
    {
        unsigned int width = static_cast<unsigned int>(data[pos++]);
        unsigned int height = static_cast<unsigned int>(data[pos++]);
        
        png::image<png::rgba_pixel> img(height, width);
        for (unsigned long row = 0; row < height; row++) {
            for (unsigned long column = 0; column < width; column++) {
                if (pos + row*width + column > data.size() - 1) {
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
        img.write(filePath);
    }

}
