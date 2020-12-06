//
// Created by shishqa on 4/15/20.
//

#ifndef HASHTABLE_CLASS_HASHTABLE_H
#define HASHTABLE_CLASS_HASHTABLE_H

#include <cstdio>
#include <utility>
#include <stdexcept>

template <class Key,
        class T,
        class Hash = std::hash<Key>,
        class KeyEqual = std::equal_to<Key>>
class HashTable {

protected:

    //Default hashtable size
    static const size_t DEFAULT_BUCKET_COUNT = 0x7FFFF;     // == 524287 (primary)
    const size_t BUCKET_COUNT;

public:

    //Types
    typedef Key key_t;
    typedef T mapped_t;

    struct Value;
    typedef Value value_t;

    //Utility
    struct Node;
    typedef Node node_t;

    class Bucket;

    typedef Bucket bucket_t;

    template <typename ValueType>
    class HashTableIterator;

    typedef HashTableIterator<bucket_t> iterator;
    typedef HashTableIterator<const bucket_t> const_iterator;

    //Constructors
    explicit HashTable(size_t bucket_count = DEFAULT_BUCKET_COUNT);

    HashTable(std::initializer_list<std::pair<key_t, mapped_t>> init,
              const size_t& bucket_count);

    HashTable(const HashTable& other);

    HashTable& operator=(const HashTable& other);

    //Destructor
    ~HashTable();

    //Iterators
    iterator begin() noexcept;

    iterator end() noexcept;

    const_iterator begin() const noexcept;

    const_iterator end() const noexcept;

    //Lookup
    mapped_t& operator[](const key_t& key);

    const mapped_t& operator[](const key_t& key) const;

    mapped_t* find(const key_t& key);

    const mapped_t* find(const key_t& key) const;

    bool count(const key_t& key) const;


    //Modification
    const value_t& insert(const value_t& value);

    void erase(const key_t& key);

    void clear();

    //Info
    size_t size() const;

protected:
    Bucket* hash_table;
    size_t current_size;
};


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::HashTable(const size_t bucket_count) :
        BUCKET_COUNT(bucket_count),
        current_size(0) {

    hash_table = new Bucket[BUCKET_COUNT];
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::HashTable(const HashTable& other) :
        BUCKET_COUNT(other.BUCKET_COUNT),
        current_size(other.current_size) {

    hash_table = static_cast<bucket_t*>(::operator new(sizeof(Bucket[other.BUCKET_COUNT])));

    for (size_t i = 0; i < BUCKET_COUNT; ++i) {

        new(hash_table + i) Bucket(other.hash_table[i]);

    }
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>&
HashTable<Key, T, Hash, KeyEqual>::operator=(const HashTable<Key, T, Hash, KeyEqual>& other) {

    if (&other == this) {
        return *this;
    }

    if (BUCKET_COUNT != other.BUCKET_COUNT) {
        throw std::runtime_error("Bad hashtable assignment (size is different)\n");
    }

    this->clear();

    this->current_size = other.current_size;

    for (size_t i = 0; i < BUCKET_COUNT; ++i) {
        new(hash_table + i) Bucket(other.hash_table[i]);
    }
}

template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::HashTable::HashTable(std::initializer_list<std::pair<key_t, mapped_t>> init,
                                                        const size_t& bucket_count) :
        BUCKET_COUNT(bucket_count),
        current_size(init.size()) {

    hash_table = new Bucket[BUCKET_COUNT];

    for (auto val : init) {

        insert(val);

    }
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::~HashTable() {

    delete[] hash_table;
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::iterator HashTable<Key, T, Hash, KeyEqual>::begin() noexcept {

    return iterator(hash_table);
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::const_iterator HashTable<Key, T, Hash, KeyEqual>::begin() const noexcept {

    return const_iterator(hash_table);
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::iterator HashTable<Key, T, Hash, KeyEqual>::end() noexcept {

    return iterator(hash_table + BUCKET_COUNT);
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::const_iterator HashTable<Key, T, Hash, KeyEqual>::end() const noexcept {

    return const_iterator(hash_table + BUCKET_COUNT);
}


template <class Key, class T, class Hash, class KeyEqual>
const typename HashTable<Key, T, Hash, KeyEqual>::mapped_t*
HashTable<Key, T, Hash, KeyEqual>::find(const key_t& key) const {

    const Hash hasher;

    size_t idx = 0;

    if (((-BUCKET_COUNT) ^ UINT64_MAX) == BUCKET_COUNT - 1) {
        idx = hasher(key) & (BUCKET_COUNT - 1);
    } else {
        idx = hasher(key) % BUCKET_COUNT;
    }

    return hash_table[idx].find_data(key);
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::mapped_t*
HashTable<Key, T, Hash, KeyEqual>::find(const key_t& key) {

    const mapped_t* data = static_cast<const HashTable<Key, T, Hash, KeyEqual>*>(this)->find(key);
    return const_cast<mapped_t*>(data);

}


template <class Key, class T, class Hash, class KeyEqual>
const typename HashTable<Key, T, Hash, KeyEqual>::mapped_t&
HashTable<Key, T, Hash, KeyEqual>::operator[](const key_t& key) const {

    const mapped_t* found = find(key);

    if (!found) {

        throw std::runtime_error("bad using of operator[] in const class");

    } else {

        return *found;

    }
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::mapped_t&
HashTable<Key, T, Hash, KeyEqual>::operator[](const key_t& key) {

    mapped_t* found = find(key);

    if (!found) {

        const value_t& inserted = insert({key, {}});

        found = const_cast<mapped_t*>(&inserted.data);
    }

    return *found;
}


template <class Key, class T, class Hash, class KeyEqual>
bool HashTable<Key, T, Hash, KeyEqual>::count(const key_t& key) const {

    return !(find(key) == nullptr);
}


template <class Key, class T, class Hash, class KeyEqual>
const typename HashTable<Key, T, Hash, KeyEqual>::value_t&
HashTable<Key, T, Hash, KeyEqual>::insert(const value_t& value) {

    const Hash hasher;

    size_t idx = hasher(value.key) % BUCKET_COUNT;

    size_t old_size = hash_table[idx].size();

    const value_t& val = hash_table[idx].insert(value);

    (old_size < hash_table[idx].size() ? ++current_size : current_size);

    return val;
}


template <class Key, class T, class Hash, class KeyEqual>
void HashTable<Key, T, Hash, KeyEqual>::erase(const key_t& key) {

    const Hash hasher;

    hash_table[hasher(key) % BUCKET_COUNT].erase(key);
}


template <class Key, class T, class Hash, class KeyEqual>
void HashTable<Key, T, Hash, KeyEqual>::clear() {

    for (size_t i = 0; i < BUCKET_COUNT; ++i) {
        hash_table[i].clear();
    }
    current_size = 0;

}

template <class Key, class T, class Hash, class KeyEqual>
size_t HashTable<Key, T, Hash, KeyEqual>::size() const {

    return current_size;
}


#endif //HASHTABLE_CLASS_HASHTABLE_H
