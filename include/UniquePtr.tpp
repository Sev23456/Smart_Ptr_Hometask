#pragma once

#include <type_traits>

template <typename T> UniquePtr<T>::UniquePtr(T* p) : ptr(p) {}

template <typename T> UniquePtr<T>::UniquePtr(UniquePtr&& other) noexcept : ptr(other.release()) {}

template <typename T> UniquePtr<T>::~UniquePtr() {
    delete ptr;
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

// U* приводится к T*, только если U — наследник T, иначе static_assert даёт ошибку компиляции
template <typename T>
template <typename U>
UniquePtr<T>::UniquePtr(UniquePtr<U>&& other) noexcept : ptr(other.release()) {
    static_assert(std::is_convertible_v<U*, T*>, "UniquePtr<U> converts to UniquePtr<T> only if U is derived from T");
}

// Проверка this != &other не нужна: UniquePtr<U> и UniquePtr<T> — разные объекты
template <typename T>
template <typename U>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<U>&& other) noexcept {
    static_assert(std::is_convertible_v<U*, T*>, "UniquePtr<U> converts to UniquePtr<T> only if U is derived from T");
    reset(other.release());
    return *this;
}

template <typename T> T& UniquePtr<T>::operator*() const {
    return *ptr;
}

template <typename T> T* UniquePtr<T>::operator->() const {
    return ptr;
}

template <typename T> T* UniquePtr<T>::get() const {
    return ptr;
}

template <typename T> T* UniquePtr<T>::release() {
    T* old = ptr;
    ptr = nullptr;
    return old;
}

template <typename T> void UniquePtr<T>::reset(T* p) {
    if (ptr != p) {
        delete ptr;
        ptr = p;
    }
}

template <typename T> UniquePtr<T[]>::UniquePtr(T* p) : ptr(p) {}

template <typename T> UniquePtr<T[]>::UniquePtr(UniquePtr&& other) noexcept : ptr(other.release()) {}

template <typename T> UniquePtr<T[]>::~UniquePtr() {
    delete[] ptr;
}

template <typename T>
UniquePtr<T[]>& UniquePtr<T[]>::operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

template <typename T> T& UniquePtr<T[]>::operator[](std::size_t index) const {
    return ptr[index];
}

template <typename T> T* UniquePtr<T[]>::get() const {
    return ptr;
}

template <typename T> T* UniquePtr<T[]>::release() {
    T* old = ptr;
    ptr = nullptr;
    return old;
}

template <typename T> void UniquePtr<T[]>::reset(T* p) {
    if (ptr != p) {
        delete[] ptr;
        ptr = p;
    }
}
