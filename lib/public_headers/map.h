/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef MAP_H
#define MAP_H

#include <vector>
#include <utility>
#include <algorithm>
#include "errorhandling.h"

namespace refude
{
    template<class V>
    struct Pair
    {
        Pair(const char *key, V& value) : key(strdup(key)), value(value)
        {
        }

        Pair(const char *key, V&& value) : key(strdup(key)), value(std::move(value))
        {
        }

        ~Pair()
        {
            if (key) {
                free(key);
            }
        }
        
        Pair(Pair& other) = delete;

        Pair(Pair&& other)
        {
            key = other.key;
            other.key = 0;
            value = std::move(other.value);
        }

        Pair& operator=(Pair& other) = delete;
        
        Pair& operator=(Pair&& other)
        {
            if (key) {
                free(key);
            }
            key = other.key;
            other.key = 0;
            value = std::move(other.value);
            return *this;
        }

        char* key;
        V value;
    };

    template<class V>
    struct Map
    {
        std::vector<Pair<V>> list;

        Map() : list(), inserting(false)
        {
        }

        Map(Map&& other) : list(), inserting(false)
        {
            if (other.inserting) throw RuntimeError("Move constructor during insert");
            this->list = move(other.list);
        }

        Map& operator=(Map&& other)
        {
            if (other.inserting || inserting) {
                throw RuntimeError("Move assignment during insert on %s", (inserting ? "this" : "other"));
            }

            this->list = move(other.list);
            return *this;
        }

        int find(const char* key) const
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

        int find(std::string key) const
        {
            return find(key.data());
        }

        Pair<V>& pairAt(int pos)
        {
            return list[pos];
        }

        int find_longest_prefix(const char* key) const
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
                list.insert(list.begin() + pos, Pair<V>(key, V()));
            }
            return list.at(pos).value;
        }

        const V& operator[](const char* key) const
        {
            bool found;
            int pos = search(key, found);
            if (! found) throw RuntimeError("Key not found: %s", key);
            return list[pos].value;
        }

        V& operator[](const std::string& key)
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
            return tmp;
        }

        V take(const std::string key) 
        {
            return take(key.data());
        }

        void erase(const char* key)
        {
            bool found;
            int pos = search(key, found);
            if (found) {
                list.erase(list.begin() + pos);
            }
        }
        
        void erase(const std::string key) 
        {
            erase(key.data()); 
        }

        void clear()
        {
            if (inserting) throw RuntimeError("Clearing map during insert");
            list.erase(list.begin(), list.end());
        }

        size_t size() const
        {
            if (inserting) throw RuntimeError("Call size during insert");
            return list.size();
        }


        void beginInsert() 
        {
            if (inserting) throw RuntimeError("Call beginInsert, but already inserting");
            inserting = true;
        }

        void endInsert() 
        {
            if (!inserting) throw RuntimeError("Call endInsert, but not inserting");
            inserting = false;
            std::stable_sort(list.begin(), list.end(), [](const Pair<V>& p1, const Pair<V>& p2) -> bool { 
                return strcmp(p1.key, p2.key) < 0; 
            });
            
            if (list.size() >= 2) {
                auto to = list.begin();
                for(auto from = list.begin() + 1; from != list.end(); from++) {
                    if (strcmp(to->key, from->key) < 0) {
                        to++; 
                    }
                    if (to != from) {
                        *to = std::move(*from);
                    }
                }
                list.erase(to + 1, list.end());
            }
        }

        void insert(const char* key, V&& value)
        {
            if (!inserting) throw RuntimeError("Call insert, but not inserting");
            list.push_back(Pair<V>(key, std::move(value)));
        }
        
        template <typename Visitor>
        void each(Visitor visitor)
        {
            for (int i = 0; i < size(); i++) {
                visitor(list.at(i).key, list.at(i).value);
            }
        }

    private:
        bool inserting;


        

        /**
         * 
         * @param key The key to search for
         * @param found Will be set to true if key is found, false otherwise
         * @return The position of the first key which is lexically greater than or
         *         equal to the given key or, if none such exists, size().
         *         This returned value is where the given key would be inserted in the map.
         */
        int search(const char * key, bool& found) const
        {
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
