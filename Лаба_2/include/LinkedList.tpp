#pragma once

#include <stdexcept>

// Возвращает узел с номером index, индекс должен быть уже проверен
template <typename T> typename LinkedList<T>::Node* LinkedList<T>::get_node(int index) const {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current;
}

// Удаляет все узлы и делает список пустым
template <typename T> void LinkedList<T>::clear() {
    while (head != nullptr) {
        Node* next = head->next;
        delete head;
        head = next;
    }
    tail = nullptr;
    length = 0;
}

template <typename T> LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0) {}

template <typename T> LinkedList<T>::LinkedList(const T* items, int count) : head(nullptr), tail(nullptr), length(0) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }
    if (count > 0 && items == nullptr) {
        throw std::invalid_argument("Items cannot be null");
    }

    for (int index = 0; index < count; index++) {
        append(items[index]);
    }
}

template <typename T> LinkedList<T>::LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), length(0) {
    for (Node* current = other.head; current != nullptr; current = current->next) {
        append(current->data);
    }
}

template <typename T> LinkedList<T>::~LinkedList() {
    clear();
}

template <typename T> LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other) {
    if (this != &other) {
        clear();
        for (Node* current = other.head; current != nullptr; current = current->next) {
            append(current->data);
        }
    }
    return *this;
}

template <typename T> const T& LinkedList<T>::get(int index) const {
    if (index < 0 || index >= length) {
        throw std::out_of_range("Index out of range");
    }
    return get_node(index)->data;
}

template <typename T> const T& LinkedList<T>::operator[](int index) const {
    return get(index);
}

template <typename T> int LinkedList<T>::get_length() const {
    return length;
}

template <typename T> void LinkedList<T>::append(const T& item) {
    Node* node = new Node{item, nullptr};
    if (tail == nullptr) {
        head = node;
    } else {
        tail->next = node;
    }
    tail = node;
    length++;
}

template <typename T> void LinkedList<T>::prepend(const T& item) {
    head = new Node{item, head};
    if (tail == nullptr) {
        tail = head;
    }
    length++;
}

template <typename T> void LinkedList<T>::insert(const T& item, int index) {
    if (index < 0 || index > length) {
        throw std::out_of_range("Index out of range");
    }
    if (index == 0) {
        prepend(item);
        return;
    }
    if (index == length) {
        append(item);
        return;
    }

    Node* previous = get_node(index - 1);
    previous->next = new Node{item, previous->next};
    length++;
}

template <typename T> void LinkedList<T>::remove(int index) {
    if (index < 0 || index >= length) {
        throw std::out_of_range("Index out of range");
    }

    if (index == 0) {
        Node* removed = head;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        }
        delete removed;
        length--;
        return;
    }

    Node* previous = get_node(index - 1);
    Node* removed = previous->next;
    previous->next = removed->next;
    if (removed == tail) {
        tail = previous;
    }
    delete removed;
    length--;
}

template <typename T> LinkedList<T> LinkedList<T>::get_sublist(int start_index, int end_index) const {
    if (start_index < 0 || end_index >= length || start_index > end_index) {
        throw std::out_of_range("Index out of range");
    }

    LinkedList<T> sublist;
    Node* current = get_node(start_index);
    for (int i = start_index; i <= end_index; i++) {
        sublist.append(current->data);
        current = current->next;
    }
    return sublist;
}

// Переносит узлы other в конец этого списка без копирования, other становится пустым
template <typename T> LinkedList<T>* LinkedList<T>::concat(LinkedList<T>& other) {
    if (&other == this) {
        throw std::invalid_argument("Cannot concat list with itself");
    }
    if (other.head == nullptr) {
        return this;
    }

    if (head == nullptr) {
        head = other.head;
    } else {
        tail->next = other.head;
    }
    tail = other.tail;
    length += other.length;

    other.head = nullptr;
    other.tail = nullptr;
    other.length = 0;
    return this;
}
