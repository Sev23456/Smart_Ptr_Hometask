#pragma once

#include <stdexcept>

template <typename T>
ListSequence<T>::Enumerator::Enumerator(const LinkedList<T>& list) : list(list), current(nullptr), next(list.head) {}

template <typename T> bool ListSequence<T>::Enumerator::move_next() {
    if (next == nullptr) {
        current = nullptr;
        return false;
    }
    current = next;
    next = next->next;
    return true;
}

template <typename T> const T& ListSequence<T>::Enumerator::get_curr() const {
    if (current == nullptr) {
        throw std::out_of_range("Enumerator is not positioned on an element");
    }
    return current->data;
}

template <typename T> bool ListSequence<T>::Enumerator::reset() {
    current = nullptr;
    next = list.head;
    return true;
}

template <typename T> ListSequence<T>::ListSequence() : items() {}

template <typename T> ListSequence<T>::ListSequence(const T* items, int count) : items(items, count) {}

template <typename T> ListSequence<T>::ListSequence(const ListSequence<T>& other) : items(other.items) {}

template <typename T> const T& ListSequence<T>::get_first() const {
    if (items.get_length() == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(0);
}

template <typename T> const T& ListSequence<T>::get_last() const {
    if (items.get_length() == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(items.get_length() - 1);
}

template <typename T> const T& ListSequence<T>::get(int index) const {
    return items.get(index);
}

template <typename T> Option<T> ListSequence<T>::try_get_first() const {
    if (items.get_length() == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(0));
}

template <typename T> Option<T> ListSequence<T>::try_get_last() const {
    if (items.get_length() == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(items.get_length() - 1));
}

template <typename T> Option<T> ListSequence<T>::try_get(int index) const {
    if (index < 0 || index >= items.get_length()) {
        return Option<T>();
    }
    return Option<T>(items.get(index));
}

template <typename T> int ListSequence<T>::get_length() const {
    return items.get_length();
}

template <typename T> Sequence<T>* ListSequence<T>::append(const T& item) {
    ListSequence<T>* result = instance();
    result->append_to_current(item);
    return result;
}

template <typename T> Sequence<T>* ListSequence<T>::prepend(const T& item) {
    ListSequence<T>* result = instance();
    result->items.prepend(item);
    return result;
}

template <typename T> Sequence<T>* ListSequence<T>::insert(const T& item, int index) {
    ListSequence<T>* result = instance();
    result->items.insert(item, index);
    return result;
}

template <typename T> Sequence<T>* ListSequence<T>::remove(int index) {
    ListSequence<T>* result = instance();
    result->items.remove(index);
    return result;
}

template <typename T> IEnumerator<T>* ListSequence<T>::get_enumerator() const {
    return new Enumerator(items);
}

template <typename T> void ListSequence<T>::append_to_current(const T& item) {
    items.append(item);
}

template <typename T> ListSequence<T>* MutableListSequence<T>::instance() {
    return this;
}

template <typename T> ListSequence<T>* MutableListSequence<T>::clone() const {
    return new MutableListSequence<T>(*this);
}

template <typename T> ListSequence<T>* MutableListSequence<T>::empty_clone() const {
    return new MutableListSequence<T>();
}

template <typename T> MutableListSequence<T>::MutableListSequence() : ListSequence<T>() {}

template <typename T>
MutableListSequence<T>::MutableListSequence(const T* items, int count) : ListSequence<T>(items, count) {}

template <typename T>
MutableListSequence<T>::MutableListSequence(const ListSequence<T>& other) : ListSequence<T>(other) {}

template <typename T> ListSequence<T>* ImmutableListSequence<T>::instance() {
    return clone();
}

template <typename T> ListSequence<T>* ImmutableListSequence<T>::clone() const {
    return new ImmutableListSequence<T>(*this);
}

template <typename T> ListSequence<T>* ImmutableListSequence<T>::empty_clone() const {
    return new ImmutableListSequence<T>();
}

template <typename T> ImmutableListSequence<T>::ImmutableListSequence() : ListSequence<T>() {}

template <typename T>
ImmutableListSequence<T>::ImmutableListSequence(const T* items, int count) : ListSequence<T>(items, count) {}

template <typename T>
ImmutableListSequence<T>::ImmutableListSequence(const ListSequence<T>& other) : ListSequence<T>(other) {}
