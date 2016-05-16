/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#ifndef TYPES_H
#define TYPES_H
#include <string>
#include <map>
namespace org_restfulipc
{
    /**
     * Maps from an icon name to a Json. The Json should be an array of Json objects, each describing an
     * icon instance. Such an object could look like:
     * <pre>
     * {
     *     "Context": "MimeTypes", 
     *     "maxSize": 50, 
     *     "mimetype": "image/png", 
     *     "minSize": 46, 
     *     "path": "/usr/share/icons/oxygen/base/48x48/places/folder.png"
     * }
     * </pre> 
     */ 
    typedef Map<Json> IconMap; 

    /**
     * Maps from a theme name (eg: 'oxygen', 'gnome', 'hicolor') to an IconMap
     */
    typedef Map<IconMap> ThemeIconMap;

    /**
     * Holds inheritance information 
     * inheritancemap["oxygen"] -> "hicolor" means oxygen inherits hicolor 
     */
    typedef std::map<std::string, std::string> InheritanceMap; 
   
}
#endif /* TYPES_H */





