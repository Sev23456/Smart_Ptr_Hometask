#pragma once
#include <cstddef>

// Если объект удаляется через UniquePtr<Base>, у Base должен быть виртуальный деструктор
template <typename T>
class UniquePtr {
private:
    T* ptr;
public:
    UniquePtr(T* p = nullptr);
    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr(UniquePtr&& other) noexcept;
    ~UniquePtr();

    UniquePtr& operator=(const UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr&& other) noexcept;

    // Подтипизация: UniquePtr<Derived> → UniquePtr<Base>, только перемещением
    template <typename U>
    UniquePtr(UniquePtr<U>&& other) noexcept;
    template <typename U>
    UniquePtr& operator=(UniquePtr<U>&& other) noexcept;

    T& operator*() const;
    T* operator->() const;
    T* get() const;

    T* release();
    void reset(T* p = nullptr);
};

// Тут копирование удалено, только get, release и reset 

template <typename T>
class UniquePtr<T[]> {
private:
    T* ptr;
public:
    UniquePtr(T* p = nullptr);
    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr(UniquePtr&& other) noexcept;
    ~UniquePtr();

    UniquePtr& operator=(const UniquePtr& other) = delete;
    UniquePtr& operator=(UniquePtr&& other) noexcept;

    T& operator[](std::size_t index) const;
    T* get() const;

    T* release();
    void reset(T* p = nullptr);
};

#include "UniquePtr.tpp"
