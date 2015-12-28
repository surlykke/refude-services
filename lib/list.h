#ifndef LIST_H
#define LIST_H

#include "errorhandling.h"

namespace org_restfulipc
{

    template<typename ValueType>
    struct List
    {
        int size;
        int capacity;
        ValueType data[];
    };

    template<typename ValueType>
    void list_append(List<ValueType>*& list, ValueType&& t)
    {
        ensureCapacityForOneMore(list);
        list->data[list->size++] = std::move(t);
    }

    template<typename ValueType>
    void list_insert(List<ValueType>*& list, ValueType&& value, int pos)
    {
        if (pos<0 || pos>list->size) throw RuntimeError("Out of range");
        ensureCapacityForOneMore(list);
        for (int j = list->size; j > pos; j--) {
            list->data[j] = std::move(list->data[j-1]);
        }
        list->data[pos] = std::move(value);
        list->size++;
    }

    template<typename ValueType>
    ValueType&& list_take(List<ValueType>*& list, int pos)
    {
        if (pos<0 || pos>=list->size) throw RuntimeError("Out of range");
        ValueType tmp = std::move(list->data[pos]);
        for (int j = pos; j < list->size-1; j++) {
            list->data[j] = std::move(list->data[j+1]);
        }
        list->data[list->size--].~ValueType();
        return std::move(tmp);
    }

    template<typename ValueType>
    ValueType&& list_take_back(List<ValueType>*& list) {
        if (!list->size) throw RuntimeError("take_back called on empty list");
        return std::move(list->data[--list->size]);
    }

    template<typename ValueType>
    ValueType& list_at(List<ValueType>*& list, int pos) {
        if (pos<0 || pos>= list->size) throw RuntimeError("Out of range");
        return list->data[pos];
    }


    template<typename ValueType>
    List<ValueType>* list_create(int initialCapacity = 4)
    {
        List<ValueType>* list = (List<ValueType>*)malloc(sizeof(List<ValueType>) + initialCapacity*sizeof(ValueType));
        if (!list) throw C_Error();
        list->size = 0;
        list->capacity = initialCapacity;
        return list;
    }

    template<typename ValueType>
    void list_delete(List<ValueType>*& list)
    {
        for (int i = 0; i < list->size; i++) {
            list->data[i].~ValueType();
        }
        free(list);
    }

    template<typename ValueType>
    void ensureCapacityForOneMore(List<ValueType>*& list)
    {
        if (list->size < list->capacity) return;

        int newCapacity = 2*list->capacity;
        list = (List<ValueType>*) realloc(list, sizeof(List<ValueType>) + newCapacity*sizeof(ValueType));
        if (!list) throw C_Error();
        memset(list->data + list->capacity*sizeof(ValueType), 0, list->capacity*sizeof(ValueType));
        list->capacity = newCapacity;
    }
}


#endif // LIST_H
