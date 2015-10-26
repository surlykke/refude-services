/* 
 * File:   json.h
 * Author: christian
 *
 * Created on 23. juli 2015, 10:32
 */

#ifndef JSON_H
#define    JSON_H

#include "heap.h"
#include <iostream>
#include <string.h>

#include "jsontypes.h"

namespace org_restfulipc 
{
    // We don't have a dedicated type for NULL, but use a null
    // string-pointer for that.

    class JsonDoc 
    {
    public:
        JsonDoc() : pRoot(0), pHeap() { }
        virtual ~JsonDoc() {}


        template<typename JsonType>
        JsonType* add(JsonArray* arr, const JsonType& value) {
            JsonType* valueCopy = new(pHeap.allocate<JsonType>()) JsonType(value);
            JsonArrayEntry* newEntry = new(pHeap.allocate<JsonArrayEntry>()) JsonArrayEntry();
            newEntry->element = valueCopy;

            if (arr->last) {
                arr->last->next = newEntry;
            }
            else {
                arr->first = newEntry;
            }

            arr->last = newEntry;
            return valueCopy;
        }

        template<typename JsonType>
        JsonType* add(JsonObject* obj, const char* key, const JsonType& value) {
            JsonType* valueCopy = new(pHeap.allocate<JsonType>()) JsonType(value);
            JsonObjectEntry* newEntry = new(pHeap.allocate<JsonObjectEntry>()) JsonObjectEntry();
            newEntry->key = key;
            newEntry->value = valueCopy;

            if (obj->last) {
                obj->last->next = newEntry;
            }
            else {
                obj->first = newEntry;
            }

            obj->last = newEntry;
            return valueCopy;
        }

        AbstractJson* root() { return pRoot; }

    private:
        
        AbstractJson* pRoot;
        Heap pHeap;        
    };


    /**
     * Specialized JsonDoc to a Hal document, meaning:
     *    - the root element is an object
     *  - knows about links
     */
    class HalJsonDoc: public JsonDoc
    {
    public:
        HalJsonDoc() : JsonDoc() {}

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

