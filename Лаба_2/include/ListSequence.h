#pragma once

#include "Sequence.h"
#include "LinkedList.h"
#include "IEnumerator.h"

template <typename T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T> items;

    ListSequence<T>* instance() override = 0;
    ListSequence<T>* clone() const override = 0;
    ListSequence<T>* empty_clone() const override = 0;

    void append_to_current(const T& item) override;
public:
    // Идёт по узлам списка, поэтому весь обход занимает O(n), а не O(n^2) через get(i)
    class Enumerator : public IEnumerator<T> {
    private:
        using Node = typename LinkedList<T>::Node;

        const LinkedList<T>& list;
        const Node* current;
        const Node* next;
    public:
        Enumerator(const LinkedList<T>& list);

        bool move_next() override;
        const T& get_curr() const override;
        bool reset() override;
    };

    ListSequence();
    ListSequence(const T* items, int count);
    ListSequence(const ListSequence<T>& other);

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
class MutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* instance() override;
    ListSequence<T>* clone() const override;
    ListSequence<T>* empty_clone() const override;
public:
    MutableListSequence();
    MutableListSequence(const T* items, int count);
    MutableListSequence(const ListSequence<T>& other);
};

// Операции возвращают изменённую копию, исходная последовательность не меняется
template <typename T>
class ImmutableListSequence : public ListSequence<T> {
protected:
    ListSequence<T>* instance() override;
    ListSequence<T>* clone() const override;
    ListSequence<T>* empty_clone() const override;
public:
    ImmutableListSequence();
    ImmutableListSequence(const T* items, int count);
    ImmutableListSequence(const ListSequence<T>& other);
};

#include "ListSequence.tpp"
