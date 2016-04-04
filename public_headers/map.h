/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef MAP_H
#define MAP_H

#include <vector>
#include <utility>
#include <algorithm>
#include "errorhandling.h"

using namespace std;

namespace org_restfulipc
{ 
    template<class V>
    struct Pair
    {
        char* key;
        V value;
    };

    template<typename V>    
    struct LessThan
    {
        bool operator()(const Pair<V>& p1, const Pair<V>& p2) 
        {
            return strcmp(p1.key, p2.key) < 0; 
        }
    };

    template<typename V>
    struct Equal
    {
        bool operator()(const Pair<V>& p1, const Pair<V>& p2) 
        {
            return strcmp(p1.key, p2.key) == 0;
        }
    };


    template<class V> 
    struct Map 
    {
        std::vector<Pair<V>> list;
        size_t sorted; 

        Map(): list(), sorted(0) {}

        V& add(const char* key, V& value, bool ownKey = false)
        {
            if (!key) throw RuntimeError("NULL is not allowed as map key");
            list.push_back({strdup(key), value});
            return list.back().value;
        }
       
        V& add(const char* key, V&& value, bool ownKey = false)
        {
            if (!key) throw RuntimeError("NULL is not allowed as map key");
            list.push_back({strdup(key), std::move(value)});
            return list.back().value;
        } 

        int find(const char* key)
        {
            bool found;
            int pos = search(key, found);
            if (found) {
                return pos;
            }
            else {
                return -1;
            }
        }

        int find_longest_prefix(const char* key)
        {
            bool found;
            int pos = search(key, found);
            if (found) {
                return pos;
            }
            else if (pos > 0 && !strncmp(key, list.at(pos - 1).key, strlen(list.at(pos - 1).key))) {
                return pos - 1;
            }
            else {
                return -1;
            }
        }

        V& operator[](const char* key)
        {
            bool found;
            int pos = search(key, found);
            if (!found) {
                list.insert(list.begin() + pos, {strdup(key), V()});
                sorted++;
            }
            return list.at(pos).value;
        }

        V& operator[](const string& key) 
        {
            return operator[](key.data());
        }
       
        V take(const char* key)
        {
            bool found;
            int pos = search(key, found);
            if (!found) throw RuntimeError("Key not found: %s", key);
            V tmp = std::move(list[pos].value);
            free(list[pos].key);
            list.erase(list.begin() + pos);
            sorted--;
            return tmp;
        }

        void erase(const char* key) 
        {
            bool found;
            int pos = search(key, found);
            if (found) {
                free(list[pos].key);
                list.erase(list.begin() + pos);
                sorted--; 
            }
        }

        void clear() 
        {
            for (auto it = list.begin(); it != list.end(); it++) {
                free(it->key);
            }
            list.erase(list.begin(), list.end());
            sorted = 0;
        }
   
        size_t size() 
        {
            sort();
            return list.size();
        }

        Pair<V> & at(size_t pos) 
        {
            sort();
            return list.at(pos);
        }

        void sort() 
        {
            if (sorted < list.size()) {
                if (sorted < list.size() - 1) {
                    std::sort(list.begin() + sorted, list.end(), LessThan<V>());
                } 
                if (sorted > 0) {
                    std::inplace_merge(list.begin(), list.begin() + sorted, list.end(), LessThan<V>());
                }
                list.erase(std::unique(list.begin(), list.end(), Equal<V>()), list.end());
            } 
            sorted = list.size();
        }

        vector<const char*> keys() 
        {
            vector<const char*> res;
            for (int i = 0; i < size(); i++) {
                res.push_back(list.at(i).key);
            }
            return res;
        }

        vector<V*> values() 
        {
            vector<V*> res;
            for (int i = 0; i < size(); i++) {
                res.push_back(&(list.at(i).value));
            }
            return res;
        }

        vector<const char*> keys(const char* prefix) {
            bool dummy; 
            int prefixLength = strlen(prefix);
            int pos = search(prefix, dummy);
            vector<const char*> res;
            while (pos < size() && !strncmp(prefix, list.at(pos).key, prefixLength)) {
                res.push_back(list.at(pos).key);
                pos++;
            }

            return res;
        }

    private:
        
        /**
         * 
         * @param key The key to search for
         * @param found Will be set to true if key is found, false otherwise
         * @return The position of the first key which is lexically greater than or
         *         equal to the given key or, if none such exists, size().
         *         This returned value is where the given key would be inserted in the map.
         */ 
        int search(const char * key, bool& found) 
        {
            sort();
            int lo = -1;
            int hi = list.size();
            while (lo < hi - 1) {
                int mid = (lo + hi)/2;
                int comp = strcmp(key, list.at(mid).key);
                if (comp == 0) {
                    found = true;
                    return mid;
                }
                else if (comp > 0) {
                    lo = mid;
                }
                else {
                    hi = mid;
                }
            }
           
            found = false;
            return hi;
        }
    };
}
#endif // MAP_H
