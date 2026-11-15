#pragma once

#include "../../include/UniquePtr.h"

template <typename T>
class DynamicArray {
private:
    UniquePtr<T[]> data;
    int size;

    void copy_from(const T* items, int count);
public:
    DynamicArray();
    DynamicArray(int initial_size);
    DynamicArray(const T* items, int count);
    DynamicArray(const DynamicArray<T>& other);

    DynamicArray<T>& operator=(const DynamicArray<T>& other);

    const T& get(int index) const;
    const T& operator[](int index) const;
    int get_size() const;

    void set(int index, const T& value);
    void resize(int new_size);
};

#include "DynamicArray.tpp"
