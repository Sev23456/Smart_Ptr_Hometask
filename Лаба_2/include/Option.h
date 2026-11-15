#pragma once

template <typename T>
class Option {
private:
    T value;
    bool contains_value;
public:
    Option();
    Option(const T& initial_value);

    bool has_value() const;

    const T& get_value() const;
    T& get_value();

    const T& operator*() const;
    T& operator*();
    const T* operator->() const;
    T* operator->();

    void set_value(const T& new_value);
    void reset();
};

#include "Option.tpp"
