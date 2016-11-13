/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef MIMEAPPSLISTREADER_H
#define MIMEAPPSLISTREADER_H
#include <ostream>
#include "typedefs.h"
/**
 * Based on the algorithm outlined in <FIXME>. 
 * Users of this class should feed it mimeapps.list files in descending order of precedence
 */
namespace refude
{
    struct MimeappsList 
    {
        MimeappsList(std::string path);
        ~MimeappsList() {}
        void write(); 
        AppLists defaultApps;
        AppSets addedAssociations;
        AppSets removedAssociations;
        
    private:
        std::string filePath;

    };
}
#endif /* MIMEAPPSLISTREADER_H */
