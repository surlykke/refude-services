/* 
 * File:   json.h
 * Author: christian
 *
 * Created on 23. juli 2015, 10:32
 */

#ifndef JSON_H
#define    JSON_H

#include <string.h>

#include <iostream>
#include <memory>
#include <cstddef>
#include <algorithm>
#include <climits>

#include "errorhandling.h"

namespace org_restfulipc 
{

    class Pimpl;

    class JsonDoc
    {
    public:
        JsonDoc(char* buf);
        virtual ~JsonDoc() {}
        void serialize(char* dest);
    private:
        Pimpl *pimpl;
    };


    /**
     * Specialized JsonDoc to a Hal document, meaning:
     *    - the root element is an object
     *  - knows about links
     */
    class HalJsonDoc: public JsonDoc
    {
    public:
        HalJsonDoc(char* buf) : JsonDoc(buf) {}

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

/*        // Some support for HAL api 
        //href, templated, type, deprecation, name, profile, title, hreflang
*/        
    

#endif    /* JSON_H */

