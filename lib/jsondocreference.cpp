#include "jsondocreference.h"

namespace org_restfulipc
{

    /*JsonReference JsonReference::operator[](const char *index)
    {
        Entry*& entry = json.firstEntry;
        while (entry && strcmp(entry->key, index)) {
            entry = entry->next;
        }
        if (! entry){
            entry = new Entry(index, JsonConst::Undefined);
        }
        return JsonReference(jsonStruct, *entry);
    }

    JsonReference JsonReference::operator[](uint32_t index)
    {
        Element*& element = json.firstElement;
        while (element && index>0) {
            element = element->next;
            index--;
        }
        if (!element) {
            throw RuntimeError("Out of range");
        }
        else {
            return JsonReference(jsonStruct, *element);
        }
    }
*/

  }
