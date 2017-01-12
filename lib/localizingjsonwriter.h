/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef LOCALIZINGJSONWRITER_H
#define LOCALIZINGJSONWRITER_H

#include <string>
#include <vector>
#include "json.h"
#include "jsonwriter.h"

namespace refude 
{
    struct LocalizingJsonWriter : public JsonWriter
    {
        LocalizingJsonWriter(Json& json, std::vector<std::string> acceptableLocales);
        virtual ~LocalizingJsonWriter();
    
    protected:
        virtual void writeKeyValue(int& written, const std::string& key, Json& value) override;
    
    private:
        std::vector<std::string> acceptableLocales;
        const char* lastResort;
    };

}

#endif /* LOCALIZINGJSONWRITER_H */

