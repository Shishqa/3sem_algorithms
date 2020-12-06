//
// Created by shishqa on 4/16/20.
//

#ifndef HASHTABLE_CLASS_NODE_H
#define HASHTABLE_CLASS_NODE_H

#include "HashTable.h"


template <class Key, class T, class Hash, class KeyEqual>
struct HashTable<Key, T, Hash, KeyEqual>::Node {

    value_t value;
    const Node* next;

    explicit Node(const value_t& value, const Node* next = nullptr);

    Node(const Node& other);

    ~Node();
};


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Node::Node(const HashTable<Key, T, Hash, KeyEqual>::value_t& value,
                                              const HashTable<Key, T, Hash, KeyEqual>::Node* next) :
        value(value),
        next(next) {
#ifdef DEBUG
    printf("Node %p created from basic constructor\n", this);
#endif
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Node::Node(const HashTable<Key, T, Hash, KeyEqual>::Node& other) :
        value(other.value),
        next(nullptr) {

    if (other.next) {

        next = new Node(*other.next);

    }

#ifdef DEBUG
    printf("Node %p created from copying constructor\n", this);
#endif
}


template <class Key, class T, class Hash, class KeyEqual>
HashTable<Key, T, Hash, KeyEqual>::Node::~Node() {

    delete next;

#ifdef DEBUG
    printf("Node %p deleted\n", this);
#endif
}


#endif //HASHTABLE_CLASS_NODE_H
