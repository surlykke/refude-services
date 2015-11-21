#ifndef HALJSONDOC_H
#define HALJSONDOC_H

#include "jsondoc.h"

namespace org_restfulipc
{
    /**
     * JsonDoc specialized to a Hal document, meaning:
     *  - the root element is an object
     *  - knows about links
     */
    class HalJsonDoc: public JsonDoc
    {
    public:
        HalJsonDoc();

        void setSelfLink(char* selfuri);
        void addRelatedLink(char* href,
                            char* profile,
                            char* name = 0);
        void addLink(char* relation,
                     char* href,
                     char* profile = 0,
                     char* name = 0);

        void addLink(char* relation,
                     char* href,
                     bool  templated,
                     char* type,
                     char* profile,
                     char* name,
                     char* title,
                     char* deprecation);
    };
}

#endif // HALJSONDOC_H
