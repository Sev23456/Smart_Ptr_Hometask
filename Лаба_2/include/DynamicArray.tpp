#pragma once

#include <stdexcept>
#include <utility>

// Заменяет содержимое массива копией первых count элементов items
template <typename T> void DynamicArray<T>::copy_from(const T* items, int count) {
    UniquePtr<T[]> replacement;
    if (count > 0) {
        replacement.reset(new T[count]);
        for (int index = 0; index < count; index++) {
            replacement[index] = items[index];
        }
    }
    data = std::move(replacement);
    size = count;
}

template <typename T> DynamicArray<T>::DynamicArray() : data(nullptr), size(0) {}

template <typename T> DynamicArray<T>::DynamicArray(int initial_size) : data(nullptr), size(0) {
    if (initial_size < 0) {
        throw std::invalid_argument("Size cannot be negative");
    }
    if (initial_size > 0) {
        data.reset(new T[initial_size]);
        size = initial_size;
    }
}

template <typename T> DynamicArray<T>::DynamicArray(const T* items, int count) : data(nullptr), size(0) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }
    if (count > 0 && items == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }
    copy_from(items, count);
}

template <typename T> DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : data(nullptr), size(0) {
    copy_from(other.data.get(), other.size);
}

template <typename T> DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        copy_from(other.data.get(), other.size);
    }
    return *this;
}

template <typename T> const T& DynamicArray<T>::get(int index) const {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

template <typename T> const T& DynamicArray<T>::operator[](int index) const {
    return get(index);
}

template <typename T> int DynamicArray<T>::get_size() const {
    return size;
}

template <typename T> void DynamicArray<T>::set(int index, const T& value) {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Index out of range");
    }
    data[index] = value;
}

template <typename T> void DynamicArray<T>::resize(int new_size) {
    if (new_size < 0) {
        throw std::invalid_argument("Size cannot be negative");
    }

    UniquePtr<T[]> replacement;
    if (new_size > 0) {
        replacement.reset(new T[new_size]);

        int will_copy = size;
        if (new_size < size) {
            will_copy = new_size;
        }
        for (int index = 0; index < will_copy; index++) {
            replacement[index] = data[index];
        }
    }

    data = std::move(replacement);
    size = new_size;
}
