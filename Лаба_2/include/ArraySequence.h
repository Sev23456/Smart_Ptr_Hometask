#pragma once

#include "Sequence.h"
#include "DynamicArray.h"
#include "IEnumerator.h"

// items — буфер с запасом, count — сколько элементов в нём реально хранится
template <typename T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T> items;
    int count;

    ArraySequence<T>* instance() override = 0;
    ArraySequence<T>* clone() const override = 0;
    ArraySequence<T>* empty_clone() const override = 0;

    void append_to_current(const T& item) override;
    void reserve(int capacity) override;
    void reallocate(int required);
public:
    class Enumerator : public IEnumerator<T> {
    private:
        const DynamicArray<T>& array;
        int current_index;
        int count;
    public:
        Enumerator(const DynamicArray<T>& array, int count);

        bool move_next() override;
        const T& get_curr() const override;
        bool reset() override;
    };

    ArraySequence();
    ArraySequence(const T* items, int count);
    ArraySequence(const ArraySequence<T>& other);

    const T& get_first() const override;
    const T& get_last() const override;
    const T& get(int index) const override;

    Option<T> try_get_first() const override;
    Option<T> try_get_last() const override;
    Option<T> try_get(int index) const override;

    int get_length() const override;

    Sequence<T>* append(const T& item) override;
    Sequence<T>* prepend(const T& item) override;
    Sequence<T>* insert(const T& item, int index) override;
    Sequence<T>* remove(int index) override;

    IEnumerator<T>* get_enumerator() const override;
};

// Операции меняют саму последовательность
template <typename T>
class MutableArraySequence : public ArraySequence<T> {
protected:
    ArraySequence<T>* instance() override;
    ArraySequence<T>* clone() const override;
    ArraySequence<T>* empty_clone() const override;
public:
    MutableArraySequence();
    MutableArraySequence(const T* items, int count);
    MutableArraySequence(const ArraySequence<T>& other);
};

// Операции возвращают изменённую копию, исходная последовательность не меняется
template <typename T>
class ImmutableArraySequence : public ArraySequence<T> {
protected:
    ArraySequence<T>* instance() override;
    ArraySequence<T>* clone() const override;
    ArraySequence<T>* empty_clone() const override;
public:
    ImmutableArraySequence();
    ImmutableArraySequence(const T* items, int count);
    ImmutableArraySequence(const ArraySequence<T>& other);
};

#include "ArraySequence.tpp"
