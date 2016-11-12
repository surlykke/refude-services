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
        virtual void writeKeyValue(int& written, const char* key, Json& value) override;
    
    private:
        std::vector<std::string> acceptableLocales;
        const char* lastResort;
    };

}

#endif /* LOCALIZINGJSONWRITER_H */

