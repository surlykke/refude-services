#include "localizingjsonwriter.h"

namespace org_restfulipc 
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

    void LocalizingJsonWriter::writeKeyValue(int& written, const char* key, Json& value)
    {
        if (!strncmp("_ripc:localized:", key, 16)) {
            key += 16;
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
                writeString(key);
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



