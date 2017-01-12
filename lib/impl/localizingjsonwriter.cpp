/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include "localizingjsonwriter.h"

namespace refude 
{
    LocalizingJsonWriter::LocalizingJsonWriter(Json& json, std::vector<std::string> acceptableLocales):
        JsonWriter(),
        acceptableLocales(acceptableLocales),
        lastResort(lastResort)
    {
        write(json);
    }

    LocalizingJsonWriter::~LocalizingJsonWriter()
    {
    }

    void LocalizingJsonWriter::writeKeyValue(int& written, const std::string& key, Json& value)
    {
        if (key == "_ripc:localized:") {
            std::string tmp = key.substr(16);
            Json* localizedValue = NULL;
            for (std::string locale : acceptableLocales) {
                if (value.contains(locale)) {
                    localizedValue = &value[locale];
                    break;
                }
            }
            if (localizedValue == NULL && value.contains("")) {
                localizedValue = &value[""];
            }

            if (localizedValue) {
                if (written) {
                    buffer.write(", ");
                }
                writeString(tmp);
                buffer.write(": ");
                write(*localizedValue);
                written++;
            }
        }
        else {
            JsonWriter::writeKeyValue(written, key, value);
        }
    }


}



