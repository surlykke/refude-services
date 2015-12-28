#ifndef MAP_H
#define MAP_H

#include <utility>

#include "errorhandling.h"

using namespace std;

namespace org_restfulipc
{

    template<typename ValueType>
    struct Pair
    {
        const char* key;
        ValueType value;
    };

    template<typename ValueType>
    struct Map
    {

        int size;
        int capacity;
        Pair<ValueType> data[];
    };

    template<typename ValueType>
    void map_insert(Map<ValueType>*& map, const char* key, ValueType&& value)
    {
        if (!key) throw RuntimeError("NULL is not allowed as map key");

        ensureCapacityForOneMore(map);
        for (int i = 0; i < map->size; i++) {
            if (! strcmp(key, map->data[i].key)) {
                map->data[i].value = std::move(value);
                return;
            }
        }

        ensureCapacityForOneMore(map);
        map->data[map->size].key = key;
        map->data[map->size++].value = std::move(value);
    }

    template<typename ValueType>
    bool map_contains(Map<ValueType>*& map, const char* key)
    {
        for (int i = 0; i<map->size; i++) {
            if (!strcmp(key, map->data[i].key)) {
                return true;
            }
        }
        return false;
    }

    template<typename ValueType>
    ValueType& map_at(Map<ValueType>*& map, const char* key)
    {
        for (int i = 0; i< map->size; i++) {
            if (!strcmp(key, map->data[i].key)) {
                return map->data[i].value;
            }
        }

        ensureCapacityForOneMore(map);
        map->data[map->size].key = key;
        new (&(map->data[map->size].value)) ValueType();
        return map->data[map->size++].value;
    }

    template<typename ValueType>
    ValueType&& map_take(Map<ValueType>*& map, const char* key)
    {
        for (int i = 0; i < map->size; i++) {
            if (!strcmp(key, map->data[i].key)) {
                ValueType tmp = std::move(map->data[i].value);
                for (int j = i+1; j < map->size; j++) {
                    map->data[j-1] = std::move(map->data[j]);
                }
                map->data[--map->size].~Pair();
                return std::move(tmp);
            }
        }

        throw RuntimeError(std::string("Key not found: ") + key);
    }

    template<typename ValueType>
    Map<ValueType>* map_create(int initialCapacity = 4)
    {
        Map<ValueType>* map = (Map<ValueType>*)malloc(sizeof(Map<ValueType>) + initialCapacity*sizeof(Pair<ValueType>));
        map->size = 0;
        map->capacity = initialCapacity;
        if (!map) throw C_Error();
        return map;
    }

    template<typename ValueType>
    void map_delete(Map<ValueType>*& map)
    {
        for (int i = 0; i < map->size; i++) {
            map->data[i].value.~ValueType();
        }

        free(map);
    }

    template<typename ValueType>
    void ensureCapacityForOneMore(Map<ValueType>*& map)
    {
        if (map->size < map->capacity) return;

        int newCapacity = 2*map->capacity;
        map = (Map<ValueType>*) realloc(map, sizeof(Map<ValueType>) + newCapacity*sizeof(Pair<ValueType>));
        if (!map) throw C_Error();
        memset(map->data + map->capacity*sizeof(Pair<ValueType>), 0, map->capacity*sizeof(Pair<ValueType>));
        map->capacity = newCapacity;
    }
}
#endif // MAP_H
