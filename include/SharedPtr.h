#pragma once
#include <cstddef>

// Если объект удаляется через SharedPtr<Base>, у Base должен быть виртуальный деструктор
template <typename T>
class SharedPtr {
private:
    // SharedPtr<Derived> и SharedPtr<Base> — разные классы, без friend они не видят поля друг друга
    template <typename U>
    friend class SharedPtr;

    T* ptr;
    std::size_t* ref_count;

    void release();
public:
    SharedPtr(T* p = nullptr);
    SharedPtr(const SharedPtr& other);
    SharedPtr(SharedPtr&& other) noexcept;
    ~SharedPtr();

    SharedPtr& operator=(const SharedPtr& other);
    SharedPtr& operator=(SharedPtr&& other) noexcept;

    // Подтипизация: SharedPtr<Derived> → SharedPtr<Base>
    template <typename U>
    SharedPtr(const SharedPtr<U>& other);
    template <typename U>
    SharedPtr(SharedPtr<U>&& other) noexcept;

    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& other);
    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) noexcept;

    T& operator*() const;
    T* operator->() const;
    T* get() const;
    std::size_t use_count() const;

    void reset(T* p = nullptr);
};

#include "SharedPtr.tpp"
