#pragma once

#include <stdexcept>

template <typename T>
ArraySequence<T>::Enumerator::Enumerator(const DynamicArray<T>& array, int count)
    : array(array), current_index(-1), count(count) {}

template <typename T> bool ArraySequence<T>::Enumerator::move_next() {
    if (current_index + 1 < count) {
        current_index++;
        return true;
    }
    current_index = count;
    return false;
}

template <typename T> const T& ArraySequence<T>::Enumerator::get_curr() const {
    if (current_index < 0 || current_index >= count) {
        throw std::out_of_range("Enumerator is not positioned on an element");
    }
    return array.get(current_index);
}

template <typename T> bool ArraySequence<T>::Enumerator::reset() {
    current_index = -1;
    return true;
}

template <typename T> ArraySequence<T>::ArraySequence() : items(), count(0) {}

template <typename T> ArraySequence<T>::ArraySequence(const T* items, int count) : items(items, count), count(count) {}

template <typename T> ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) : items(other.items), count(other.count) {}

template <typename T> const T& ArraySequence<T>::get_first() const {
    if (count == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(0);
}

template <typename T> const T& ArraySequence<T>::get_last() const {
    if (count == 0) {
        throw std::out_of_range("Sequence is empty");
    }
    return items.get(count - 1);
}

template <typename T> const T& ArraySequence<T>::get(int index) const {
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index out of range");
    }
    return items.get(index);
}

template <typename T> Option<T> ArraySequence<T>::try_get_first() const {
    if (count == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(0));
}

template <typename T> Option<T> ArraySequence<T>::try_get_last() const {
    if (count == 0) {
        return Option<T>();
    }
    return Option<T>(items.get(count - 1));
}

template <typename T> Option<T> ArraySequence<T>::try_get(int index) const {
    if (index < 0 || index >= count) {
        return Option<T>();
    }
    return Option<T>(items.get(index));
}

template <typename T> int ArraySequence<T>::get_length() const {
    return count;
}

template <typename T> Sequence<T>* ArraySequence<T>::append(const T& item) {
    ArraySequence<T>* result = instance();
    result->append_to_current(item);
    return result;
}

template <typename T> Sequence<T>* ArraySequence<T>::prepend(const T& item) {
    return insert(item, 0);
}

// Сдвигает элементы с index на одну позицию вправо и записывает item на освободившееся место
template <typename T> Sequence<T>* ArraySequence<T>::insert(const T& item, int index) {
    if (index < 0 || index > count) {
        throw std::out_of_range("Index out of range");
    }

    ArraySequence<T>* result = instance();
    result->reallocate(result->count + 1);
    for (int current = result->count; current > index; current--) {
        result->items.set(current, result->items.get(current - 1));
    }
    result->items.set(index, item);
    result->count++;
    return result;
}

template <typename T> Sequence<T>* ArraySequence<T>::remove(int index) {
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index out of range");
    }

    ArraySequence<T>* result = instance();
    for (int current = index; current < result->count - 1; current++) {
        result->items.set(current, result->items.get(current + 1));
    }
    result->count--;
    return result;
}

template <typename T> IEnumerator<T>* ArraySequence<T>::get_enumerator() const {
    return new Enumerator(items, count);
}

template <typename T> void ArraySequence<T>::append_to_current(const T& item) {
    reallocate(count + 1);
    items.set(count, item);
    count++;
}

template <typename T> void ArraySequence<T>::reserve(int capacity) {
    if (capacity < 0) {
        throw std::invalid_argument("Capacity cannot be negative");
    }
    if (capacity > items.get_size()) {
        items.resize(capacity);
    }
}

// Увеличивает буфер в 2 раза, пока в него не поместится required элементов
template <typename T> void ArraySequence<T>::reallocate(int required) {
    if (required <= items.get_size()) {
        return;
    }

    int new_capacity = items.get_size();
    if (new_capacity == 0) {
        new_capacity = 1;
    }
    while (new_capacity < required) {
        new_capacity *= 2;
    }
    items.resize(new_capacity);
}

template <typename T> ArraySequence<T>* MutableArraySequence<T>::instance() {
    return this;
}

template <typename T> ArraySequence<T>* MutableArraySequence<T>::clone() const {
    return new MutableArraySequence<T>(*this);
}

template <typename T> ArraySequence<T>* MutableArraySequence<T>::empty_clone() const {
    return new MutableArraySequence<T>();
}

template <typename T> MutableArraySequence<T>::MutableArraySequence() : ArraySequence<T>() {}

template <typename T>
MutableArraySequence<T>::MutableArraySequence(const T* items, int count) : ArraySequence<T>(items, count) {}

template <typename T>
MutableArraySequence<T>::MutableArraySequence(const ArraySequence<T>& other) : ArraySequence<T>(other) {}

template <typename T> ArraySequence<T>* ImmutableArraySequence<T>::instance() {
    return clone();
}

template <typename T> ArraySequence<T>* ImmutableArraySequence<T>::clone() const {
    return new ImmutableArraySequence<T>(*this);
}

template <typename T> ArraySequence<T>* ImmutableArraySequence<T>::empty_clone() const {
    return new ImmutableArraySequence<T>();
}

template <typename T> ImmutableArraySequence<T>::ImmutableArraySequence() : ArraySequence<T>() {}

template <typename T>
ImmutableArraySequence<T>::ImmutableArraySequence(const T* items, int count) : ArraySequence<T>(items, count) {}

template <typename T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ArraySequence<T>& other) : ArraySequence<T>(other) {}
