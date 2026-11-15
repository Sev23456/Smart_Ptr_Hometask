#pragma once

#include <type_traits>

// Уменьшает счётчик ссылок и удаляет объект, если ссылок не осталось
template <typename T> void SharedPtr<T>::release() {
    if (ref_count != nullptr) {
        --(*ref_count);
        if (*ref_count == 0) {
            delete ptr;
            delete ref_count;
        }
    }
    ptr = nullptr;
    ref_count = nullptr;
}

template <typename T> SharedPtr<T>::SharedPtr(T* p) : ptr(p), ref_count(nullptr) {
    if (p != nullptr) {
        ref_count = new std::size_t(1);
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
    if (ref_count != nullptr) {
        ++(*ref_count);
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
    other.ptr = nullptr;
    other.ref_count = nullptr;
}

template <typename T> SharedPtr<T>::~SharedPtr() {
    release();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
    if (this != &other) {
        release();
        ptr = other.ptr;
        ref_count = other.ref_count;
        if (ref_count != nullptr) {
            ++(*ref_count);
        }
    }
    return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept {
    if (this != &other) {
        release();
        ptr = other.ptr;
        ref_count = other.ref_count;
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }
    return *this;
}

// U* приводится к T*, только если U — наследник T, иначе static_assert даёт ошибку компиляции
template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(const SharedPtr<U>& other) : ptr(other.ptr), ref_count(other.ref_count) {
    static_assert(std::is_convertible_v<U*, T*>, "SharedPtr<U> converts to SharedPtr<T> only if U is derived from T");
    if (ref_count != nullptr) {
        ++(*ref_count);
    }
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(SharedPtr<U>&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
    static_assert(std::is_convertible_v<U*, T*>, "SharedPtr<U> converts to SharedPtr<T> only if U is derived from T");
    other.ptr = nullptr;
    other.ref_count = nullptr;
}

// Проверка this != &other не нужна: SharedPtr<U> и SharedPtr<T> — разные объекты
template <typename T>
template <typename U>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& other) {
    static_assert(std::is_convertible_v<U*, T*>, "SharedPtr<U> converts to SharedPtr<T> only if U is derived from T");
    release();
    ptr = other.ptr;
    ref_count = other.ref_count;
    if (ref_count != nullptr) {
        ++(*ref_count);
    }
    return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) noexcept {
    static_assert(std::is_convertible_v<U*, T*>, "SharedPtr<U> converts to SharedPtr<T> only if U is derived from T");
    release();
    ptr = other.ptr;
    ref_count = other.ref_count;
    other.ptr = nullptr;
    other.ref_count = nullptr;
    return *this;
}

template <typename T> T& SharedPtr<T>::operator*() const {
    return *ptr;
}

template <typename T> T* SharedPtr<T>::operator->() const {
    return ptr;
}

template <typename T> T* SharedPtr<T>::get() const {
    return ptr;
}

// Сколько SharedPtr владеют объектом, у пустого указателя 0
template <typename T> std::size_t SharedPtr<T>::use_count() const {
    if (ref_count == nullptr) {
        return 0;
    }
    return *ref_count;
}

template <typename T> void SharedPtr<T>::reset(T* p) {
    if (p != ptr) {
        release();
        if (p != nullptr) {
            ptr = p;
            ref_count = new std::size_t(1);
        }
    }
}
