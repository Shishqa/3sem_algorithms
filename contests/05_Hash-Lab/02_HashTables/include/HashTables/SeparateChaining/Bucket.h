//
// Created by shishqa on 4/15/20.
//

#ifndef HASHTABLE_CLASS_BUCKET_H
#define HASHTABLE_CLASS_BUCKET_H

#include "HashTable.h"

template <class Key, class T, class Hash, class KeyEqual>
class HashTable<Key, T, Hash, KeyEqual>::Bucket : public Node {
public:
    //Constructors
    Bucket();

    Bucket(const Bucket& other);

    //Destructor
    ~Bucket();

    //Interface
    mapped_t* find_data(const key_t& key) const;

    node_t* find_node(const key_t& key) const;

    const value_t& insert(const value_t& value);

    void erase(const key_t& key);

    void clear();

    size_t size();

    const node_t* end();

private:
    size_t current_size;
    const node_t* last_node;
};


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Bucket::Bucket() :
        Node(Value(), nullptr),
        current_size(0),
        last_node(this) {
#ifdef DEBUG
    printf("Bucket %p created from basic constructor\n", this);
#endif
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Bucket::Bucket(const bucket_t& other) :
        Node(other),
        current_size(other.current_size) {

    for (last_node = this; last_node->next != nullptr; last_node = last_node->next);

#ifdef DEBUG
    printf("Bucket %p created from copying constructor\n", this);
#endif
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Bucket::~Bucket() {
#ifdef DEBUG
    printf("Bucket %p deleted\n", this);
#endif
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::node_t*
HashTable<Key, T, Hash, KeyEqual>::Bucket::find_node(const key_t& key) const {

    const node_t* it = this;
    const KeyEqual key_eq;

    while (it != last_node) {

        if (key_eq(it->value.key, key)) {
            return const_cast<node_t*>(it);
        }

        it = it->next;
    }

    return nullptr;
}


template <class Key, class T, class Hash, class KeyEqual>
typename HashTable<Key, T, Hash, KeyEqual>::mapped_t*
HashTable<Key, T, Hash, KeyEqual>::Bucket::find_data(const key_t& key) const {

    Node* found_node = find_node(key);

    if (!found_node) {
        return nullptr;
    }

    return &(found_node->value.data);
}


template <class Key, class T, class Hash, class KeyEqual>
const typename HashTable<Key, T, Hash, KeyEqual>::value_t&
HashTable<Key, T, Hash, KeyEqual>::Bucket::insert(const value_t& value) {

    Node* found_node = find_node(value.key);

    if (found_node) {

        found_node->value.data = value.data;

        return found_node->value;

    } else {

        const Node* this_moved = new Node(this->value, this->next);

        this->next = this_moved;
        this->value = value;

        if (!current_size) {
            this->last_node = this_moved;
        }

        ++current_size;

        return this->value;
    }
}

template <class Key, class T, class Hash, class KeyEqual>
void HashTable<Key, T, Hash, KeyEqual>::Bucket::erase(const key_t& key) {

    Node* found_node = find_node(key);

    if (!found_node) {
        return;
    }

    if (found_node->next == last_node) {
        last_node = found_node;
    }

    const Node* next_node = found_node->next;

    found_node->value = next_node->value;
    found_node->next = next_node->next;

    next_node->next = nullptr;
    delete next_node;
}


template <class Key, class T, class Hash, class KeyEqual>
void HashTable<Key, T, Hash, KeyEqual>::Bucket::clear() {

    delete this->next;

    last_node = this;
    current_size = 0;
}


template <class Key, class T, class Hash, class KeyEqual>
size_t HashTable<Key, T, Hash, KeyEqual>::Bucket::size() {

    return this->current_size;
}


template <class Key, class T, class Hash, class KeyEqual>
const typename HashTable<Key, T, Hash, KeyEqual>::node_t*
HashTable<Key, T, Hash, KeyEqual>::Bucket::end() {

    return this->last_node;
}


#endif //HASHTABLE_CLASS_BUCKET_H
