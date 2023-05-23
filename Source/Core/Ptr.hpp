#pragma once

#include "Types.hpp"

/// Managed unique pointer class
template <typename T>
class Ptr {
   private:
    /// Raw pointer
    T* ptr = nullptr;

   public:
    /// Default constructor. Initializes pointer to nullptr
    Ptr() = default;

    /// Nullptr constructor
    Ptr(decltype(nullptr)) {}

    /// Take ownership of raw pointer
    explicit Ptr(T* _ptr) : ptr(_ptr) {}

    /// Move constructor
    Ptr(Ptr&& p) noexcept {
        ptr = p.ptr;
        p.ptr = nullptr;
    }

    /// Construct from pointer of compatible type (ex: derived class)
    template <typename T2>
    Ptr(Ptr<T2>&& p) {
        ptr = p.get();
        p = nullptr;
    }

    /// Destructor
    ~Ptr() { delete ptr; }

    /// Move assign operator
    Ptr& operator=(Ptr&& p) noexcept {
        ptr = p.ptr;
        p.ptr = nullptr;
        return *this;
    }

    /// Nullptr assign
    Ptr& operator=(decltype(nullptr)) {
        ptr = nullptr;
        return *this;
    }

    /// Deleted copy constructor
    Ptr(Ptr&) = delete;

    /// Deleted copy assign operator
    Ptr& operator=(Ptr&) = delete;

    /// Create and take ownership of a new object
    template <typename... Args>
    static Ptr create(Args... args) {
        return Ptr(new T(args...));
    }

    /// Return the raw pointer
    T* get() { return ptr; }

    /// Conversion to bool (false if is nullptr, true otherwise)
    operator bool() const { return ptr == nullptr; }

    /// Arrow operator to return raw pointer
    T* operator->() { return get(); }
};
