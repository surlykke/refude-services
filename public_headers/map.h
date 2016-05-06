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
    template<class V, bool copyKey>
    struct Pair;

    template<class V>
    struct Pair<V, true>
    {

        Pair(const char *key, V& value) : key(strdup(key)), value(value)
        {
        }

        Pair(const char *key, V&& value) : key(strdup(key)), value(move(value))
        {
        }

        ~Pair()
        {
            if (key) {
                free(key);
            }
        }

        Pair(Pair&& other)
        {
            key = other.key;
            other.key = 0;
            value = move(other.value);
        }

        Pair& operator=(Pair&& other)
        {
            if (key) {
                free(key);
            }
            key = other.key;
            other.key = 0;
            value = move(other.value);
            return *this;
        }

        char* key;
        V value;
    };

    template<class V>
    struct Pair<V, false>
    {

        Pair(const char *key, V& value) : key(key), value(value)
        {
        }

        Pair(const char *key, V&& value) : key(key), value(move(value))
        {
        }

        ~Pair()
        {
        }

        Pair(Pair&& other)
        {
            key = other.key;
            value = move(other.value);
        }

        Pair& operator=(Pair&& other)
        {
            key = other.key;
            value = move(other.value);
            return *this;
        }

        const char* key;
        V value;
    };

    template<typename V, bool copyKey>
    struct LessThan
    {

        bool operator()(const Pair<V, copyKey>& p1, const Pair<V, copyKey>& p2)
        {
            return strcmp(p1.key, p2.key) < 0;
        }
    };

    template<typename V, bool copyKey>
    struct Equal
    {

        bool operator()(const Pair<V, copyKey>& p1, const Pair<V, copyKey>& p2)
        {
            return strcmp(p1.key, p2.key) == 0;
        }
    };

    template<class V, bool copyKey = true >
    struct Map
    {
        std::vector<Pair<V, copyKey>> list;
        size_t sorted;

        Map() : list(), sorted(0)
        {
        }

        Map(Map&& other)
        {
            this->list = move(other.list);
            this->sorted = other.sorted;
            other.sorted = 0;
        }

        Map& operator=(Map&& other)
        {
            this->list = move(other.list);
            this->sorted = other.sorted;
            other.sorted = 0;
            return *this;
        }

        V& add(const char* key, V& value, bool ownKey = false)
        {
            if (!key) throw RuntimeError("NULL is not allowed as map key");
            list.push_back(Pair<V, copyKey>(key, value));
            return list.back().value;
        }

        V& add(const char* key, V&& value, bool ownKey = false)
        {
            if (!key) throw RuntimeError("NULL is not allowed as map key");
            list.push_back(Pair<V, copyKey>(key, std::move(value)));
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

        int find(string key) 
        {
            return find(key.data());
        }

        bool contains(const char* key) 
        {
            return find(key) > -1;
        }

        bool contains(string key) 
        {
            return find(key.data()) > -1;
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
                list.insert(list.begin() + pos, Pair<V, copyKey>(key, V()));
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
            list.erase(list.begin() + pos);
            sorted--;
            return tmp;
        }

        V take(const string key) 
        {
            return take(key.data());
        }

        void erase(const char* key)
        {
            bool found;
            int pos = search(key, found);
            if (found) {
                list.erase(list.begin() + pos);
                sorted--;
            }
        }

        void clear()
        {
            list.erase(list.begin(), list.end());
            sorted = 0;
        }

        size_t size()
        {
            sort();
            return list.size();
        }

        const char* keyAt(size_t pos)
        {
            sort();
            return list.at(pos).key;
        }

        V& valueAt(size_t pos)
        {
            sort();
            return list.at(pos).value;
        }

        void sort()
        {
            if (sorted < list.size()) {
                if (sorted < list.size() - 1) {
                    std::sort(list.begin() + sorted, list.end(), LessThan<V, copyKey>());
                }
                if (sorted > 0) {
                    std::inplace_merge(list.begin(), list.begin() + sorted, list.end(), LessThan<V, copyKey>());
                }
                list.erase(std::unique(list.begin(), list.end(), Equal<V, copyKey>()), list.end());
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

        vector<const char*> keys(const char* prefix)
        {
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

        //typedef void (*Visitor)(const char*, V&);
        template <typename Visitor>
        void each(Visitor visitor)
        {
            for (int i = 0; i < size(); i++) {
                visitor(list.at(i).key, list.at(i).value);
            }
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
                int mid = (lo + hi) / 2;
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
