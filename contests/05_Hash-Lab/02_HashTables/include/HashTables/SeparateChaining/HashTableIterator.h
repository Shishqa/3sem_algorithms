//
// Created by shishqa on 4/16/20.
//

#ifndef HASHTABLE_CLASS_HASHTABLEITERATOR_H
#define HASHTABLE_CLASS_HASHTABLEITERATOR_H

#include "HashTable.h"

template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
class HashTable<Key, T, Hash, KeyEqual>::HashTableIterator
        : public std::iterator<std::forward_iterator_tag, ValueType> {
private:
    typedef HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType> iterator_t;

    friend class HashTable;

    HashTableIterator(const Bucket* curr_bucket);

public:
    HashTableIterator(const iterator_t& other);

    bool operator==(const iterator_t& other) const;

    bool operator!=(const iterator_t& other) const;

    typename iterator_t::reference operator*() const;

    iterator_t& operator++();
    //bucket_iterator_t&& operator++(int);

private:

    const Bucket* curr_bucket;
};


template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType>::HashTableIterator(const iterator_t& other) :
        curr_bucket(other.curr_bucket) {}


template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType>::HashTableIterator(const Bucket* curr_bucket) :
        curr_bucket(curr_bucket)  {}



template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
bool HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType>::operator!=(const iterator_t& other) const {
    return curr_bucket != other.curr_bucket;
}


template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
bool HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType>::operator==(const iterator_t& other) const {
    return curr_bucket == other.curr_bucket;
}


template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
typename HashTable<Key, T, Hash, KeyEqual>::template HashTableIterator<ValueType>::reference
HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType>::operator*() const {
    return *const_cast<bucket_t*>(curr_bucket);
}


template <class Key, class T, class Hash, class KeyEqual>
template <class ValueType>
typename HashTable<Key, T, Hash, KeyEqual>::template HashTableIterator<ValueType>&
HashTable<Key, T, Hash, KeyEqual>::HashTableIterator<ValueType>::operator++() {

    ++curr_bucket;
    return *this;
}


#endif //HASHTABLE_CLASS_HASHTABLEITERATOR_H
