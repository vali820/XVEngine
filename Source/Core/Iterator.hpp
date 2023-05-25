#pragma once

template <typename T>
class Iterator {
   private:
    T* ptr;

   public:
    explicit Iterator(T* ptr) noexcept : ptr(ptr) {}
    bool operator!=(Iterator rhs) { return ptr != rhs.ptr; }
    T& operator*() { return *ptr; }
    void operator++() { ptr++; }
};