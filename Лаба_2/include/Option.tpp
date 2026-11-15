#pragma once

#include <stdexcept>

template <typename T> Option<T>::Option() : value(), contains_value(false) {}

template <typename T> Option<T>::Option(const T& initial_value) : value(initial_value), contains_value(true) {}

template <typename T> bool Option<T>::has_value() const {
    return contains_value;
}

template <typename T> const T& Option<T>::get_value() const {
    if (!contains_value) {
        throw std::runtime_error("Option does not have a value");
    }
    return value;
}

template <typename T> T& Option<T>::get_value() {
    if (!contains_value) {
        throw std::runtime_error("Option does not have a value");
    }
    return value;
}

template <typename T> const T& Option<T>::operator*() const {
    return get_value();
}

template <typename T> T& Option<T>::operator*() {
    return get_value();
}

template <typename T> const T* Option<T>::operator->() const {
    return &get_value();
}

template <typename T> T* Option<T>::operator->() {
    return &get_value();
}

template <typename T> void Option<T>::set_value(const T& new_value) {
    value = new_value;
    contains_value = true;
}

template <typename T> void Option<T>::reset() {
    contains_value = false;
}
