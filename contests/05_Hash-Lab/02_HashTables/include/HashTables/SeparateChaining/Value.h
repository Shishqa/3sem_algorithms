//
// Created by shishqa on 4/16/20.
//

#ifndef HASHTABLE_CLASS_VALUE_H
#define HASHTABLE_CLASS_VALUE_H

#include "HashTable.h"

template <class Key, class T, class Hash, class KeyEqual>
struct HashTable<Key, T, Hash, KeyEqual>::Value {

    key_t key;
    T data;

    Value() = default;

    Value(const key_t& key, const mapped_t& data);

    Value(const std::pair<key_t, mapped_t>& val);

    Value(const Value& other) = default;

    ~Value() = default;

    Value& operator=(const Value& right);


};


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Value::Value(const key_t &key, const mapped_t &data) :
        key(key), data(data) {}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Value::Value(const std::pair<key_t, mapped_t>& val) :
        key(val.first), data(val.second) {}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::Value&
HashTable<Key, T, Hash, KeyEqual>::Value::operator=(const Value& right) {

    if (&right == this) {
        return *this;
    }

    this->data = right.data;
    this->key = right.key;

    return *this;
}


#endif //HASHTABLE_CLASS_VALUE_H
