/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <libgen.h>
#include <limits.h>
#include <sys/stat.h>

#include <refude/map.h>
#include <refude/utils.h>
#include <refude/jsonwriter.h>

#include "iconresource.h"
namespace refude
{

    IconResource::IconResource(ThemeIconMap&& themeIconMap, IconMap&& usrSharePixmapIcons, InheritanceMap&& inheritanceMap) :
        WebServer("/"),
        themeIconMap(std::move(themeIconMap)),
        usrSharePixmapsIcons(std::move(usrSharePixmapIcons)),
        inheritanceMap(std::move(inheritanceMap))
    {
    }

    IconResource::~IconResource()
    {
    }

    PathMimetypePair IconResource::findFile(HttpMessage& request)
    {
        std::vector<std::string> names;
        std::string themeName;
        int size;

        if (request.queryParameterMap["name"].size() < 1) throw HttpCode::Http422;
        if (request.queryParameterMap["theme"].size() > 1) throw HttpCode::Http422;
        if (request.queryParameterMap["size"].size() > 1) throw HttpCode::Http422;

        if (request.queryParameterMap["name"].size() == 1 && *(request.queryParameterMap["name"][0]) == '/') {
            // We do not support mixing names and absolute paths in one request, nor giving more than one 
            // absolute path.
            return findByPath(request.queryParameterMap["name"][0]);
        }

        if (request.queryParameterMap["theme"].size() == 1) {
            themeName = request.queryParameterMap["theme"][0];
        }
        else {
            themeName = "oxygen"; // FIXME
        }

        if (themeIconMap.find(themeName) < 0) {
            std::cerr << "No theme '" << themeName << "'\n";
            throw HttpCode::Http404;
        }

        if (request.queryParameterMap["size"].size() == 1) {
            size = std::stoi(request.queryParameterMap["size"][0]);
        }
        else {
            size = 32; // FIXME
        }

        for (std::string& name = themeName; !name.empty(); name = inheritanceMap[name]) {
            IconMap& iconMap = themeIconMap[name];
            for (const std::string& name : request.queryParameterMap["name"]) {
                int pos = iconMap.find(name);
                if (pos >= 0) {
                    Json* icon = findPathOfClosest(iconMap.pairAt(pos).value, size);
                    if (icon) {
                        return { (*icon)["path"].toString(), (*icon)["mimetype"].toString() };
                    }
                }
            }
        }

        // So no icons in theme or it's ancestors. We look for an icon in
        // /usr/share/pixmaps, where some application icons can be found
        for (std::string name : request.queryParameterMap["name"]) {
            // Sometimes references from destopfiles to icons in 
            // pixmap has endings.. (minestein: We're looking at you..) 
            if (endsWithOneOf(name.data(), {".png", ".xpm", ".svg"})) {
                name = name.substr(0, name.length() - 4);
            };
            int pos = usrSharePixmapsIcons.find(name);
            if (pos >= 0) {
                Json* icon = findPathOfClosest(usrSharePixmapsIcons.pairAt(pos).value, size);
                if (icon) { 
                    return { (*icon)["path"].toString(), (*icon)["mimetype"].toString() };
                }
            }
        }

        // Abandon all hope
        throw HttpCode::Http404;
    }

    PathMimetypePair IconResource::findByPath(const char* path)
    {
        char resolvedPath[PATH_MAX];
        if (!realpath(path, resolvedPath)) throw C_Error();

        // Check that 'others' have read permission to file and execute permission
        // to all directories above it.        
        if (!othersHavePermissions(resolvedPath, 4)) throw HttpCode::Http405;
        const char* fileName = basename(resolvedPath);
        const char* mimetype = "";
        if (strlen(fileName) > 4) {
            const char* fileEnding = fileName + strlen(fileName) - 4;
            if (!strcmp(fileEnding, ".png")) {
                mimetype = "image/png";
            }
            else if (!strcmp(fileEnding, ".xpm")) {
                mimetype = "image/x-xpixmap";
            }
            else if (!strcmp(fileEnding, ".svg")) {
                mimetype = "image/svg+xml";
            }
        }
        if (!strlen(mimetype)) {
            throw HttpCode::Http404;
        }

        do {
            if (!dirname(resolvedPath)) throw C_Error();
            if (!othersHavePermissions(resolvedPath, 1)) throw HttpCode::Http405;
        } 
        while (strcmp("/", resolvedPath));
        
        return { path, mimetype };
    }

    bool IconResource::othersHavePermissions(const char* filePath, mode_t permissions)
    {
        struct stat buf;
        if (stat(filePath, &buf) < 0) throw C_Error();
        return (buf.st_mode & permissions);
    }

    
    Json* IconResource::findPathOfClosest(Json& iconList, int size)
    {
        double bestDistanceSoFar = std::numeric_limits<double>::max();
        Json* candidate = NULL;
        for (int i = 0; i < iconList.size(); i++) {
            Json* instance = &iconList[i];
            double distance;
            double minSize = (*instance)["MinSize"].toDouble();
            double maxSize = (*instance)["MaxSize"].toDouble();
            if (size < minSize) {
                distance = minSize - size;
            }
            else if (size > maxSize) {
                distance = size - maxSize;
            }
            else {
                return instance; // No reason to look further..
            }

            if (distance < bestDistanceSoFar) {
                candidate = instance;
                bestDistanceSoFar = distance;
            }
        }
        return candidate;
    }

}
