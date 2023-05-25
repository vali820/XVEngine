#pragma once

#include <cstring>
#include <format>
#include <memory>

#include "Iterator.hpp"
#include "Types.hpp"

/**
 * @brief Dynamically allocated array container.
 *
 * @tparam T The type of the container's elements.
 */
template <typename T>
class Vec {
   private:
    T* data  = nullptr;
    u64 size = 0, capacity = 0;

   public:
    /** @brief Default constructor.
     *
     * Initializes the container as empty with size and capacity 0. Does not allocate any memory.
     */
    Vec() = default;

    /** @brief Destructor.
     *
     *  Destroys and deallocates the container's elements.
     */
    ~Vec() { destroy(); }

    /**
     * @brief Copy constructor.
     *
     * Initializes the container with copies of the other container's elements.
     * Capacity is reset to the container's size size.
     *
     * @param v Value to copy.
     */
    Vec(const Vec& v) : size(v.size), capacity(v.size) {
        allocate();
        memcpy(v.data, data, size * sizeof(T));
    }

    /**
     * @brief Move constructor.
     *
     * Initializes the container with the other container's size, capacity and data, without
     * copying any of the elements. The other container is destroyed and is effectively at
     * the state after calling the default constructor.
     *
     * @param v Value to move.
     */
    Vec(Vec&& v) noexcept : size(v.size), capacity(v.capacity), data(v.data) {
        v.data     = nullptr;
        v.size     = 0;
        v.capacity = 0;
    }

    /**
     * @brief Copy assignment.
     *
     * Destroys the container and then re-initializes it with copies of the other container's
     * elements. Capacity is reset to the container's size size.
     *
     * @param v Value to copy.
     */
    Vec& operator=(const Vec& v) {
        if (this == &v) return *this;

        destroy();

        size     = v.size;
        capacity = v.size;
        allocate();
        memcpy(data, v.data, size * sizeof(T));

        return *this;
    }

    /**
     * @brief Move assignment.
     *
     * Destroys the container and then re-initializes it with the other container's size,
     * capacity and data, without copying any of the elements. The other container is
     * destroyed and is effectively at the state after calling the default constructor.
     *
     * @param v Value to move.
     */
    Vec& operator=(Vec&& v) noexcept {
        destroy();
        size     = v.size;
        capacity = v.capacity;
        data     = v.data;
        v.data   = nullptr;
        return *this;
    }

    /**
     * @brief Initializes the container with n elements and calls default constructor for each.
     *
     * @param n Amount of elements.
     */
    explicit Vec(u64 n) : size(n), capacity(n) {
        allocate();
        callConstructors();
    }

    /**
     * @brief Initializes the container with n copies of x.
     *
     * @param n Amount of elements.
     * @param x Value to copy.
     */
    Vec(u64 n, const T& x) : size(n), capacity(n) {
        allocate();
        for (u64 i = 0; i < n; i++) data[i] = x;
    }

    Vec(std::initializer_list<T> x) : size(x.size()), capacity(x.size()) {
        allocate();
        memcpy(data, x.begin(), size * sizeof(T));
    }

    /**
     * @brief Appends an element to the end of the container.
     *
     * @param x Value to copy into the new element.
     */
    void push(const T& x) {
        if (size >= capacity) reallocate(capacity * 2);
        data[size] = x;
        size++;
    }

    /**
     * @brief Appends an element to the end of the container.
     *
     * @param x Value to move into the new element.
     */
    void push(T&& x) {
        if (size >= capacity) reallocate(capacity * 2);
        data[size] = x;
        size++;
    }

    /**
     * @brief Remove and element from the container.
     *
     * @param index The index of the item to remove.
     */
    void remove(u64 index) {
        if (index >= size) return;

        data[index].~T();
        if (index != size - 1) {  // Move elements back
            memmove(data + index, data + index + 1, (size - index - 1) * sizeof(T));
        }
        size--;
    }

    /**
     * @brief Resets the container.
     *
     * Memory is kept allocated.
     */
    void clear() {
        callDestructors();
        size = 0;
    }

    /**
     * @brief Changes the size of the container.
     *
     * If n is larger than the container's size, new elements will be created and memory will be
     * reallocated if n is also larger than the container's capacity. If n is smaller than the
     * container's size, elements will be destroyed and remaining memory is kept allocated.
     *
     * @param n New size of the container.
     */
    void resize(u64 n) {
        if (n == size) return;

        if (n > size) {  // new size is larger
            reallocate(n);
            u64 oldSize = size;
            callConstructors(oldSize);
        } else  // new size is smaller
            callDestructors(n);

        size = n;
    }

    /**
     * @brief Indexing operator.
     *
     * No out-of-bounds checking.
     *
     * @param pos Position of the element.
     * @return Reference to the element.
     */
    T& operator[](u64 pos) { return data[pos]; }

    const T& operator[](u64 pos) const { return data[pos]; }

    /**
     * @return Amount of elements in the container
     */
    [[nodiscard]] u64 getSize() const { return size; }

    /**
     * @return Amount of elements that the container has memory allocated for
     */
    [[nodiscard]] u64 getCapacity() const { return capacity; }

    /**
     * @return Raw pointer to the container's data
     */
    T* getData() { return data; }

    const T* getData() const { return data; }

    /**
     * @return Iterator to the first element of the container
     */
    [[nodiscard]] Iterator<T> begin() const { return Iterator(data); }

    /**
     * @return Iterator following the last element of the container
     */
    [[nodiscard]] Iterator<T> end() const { return Iterator(data + size); }

   private:
    void destroy() {
        if (!data) return;
        callDestructors();
        ::operator delete(data);
    }

    void reallocate(u64 n) {
        if (capacity == 0 and n == 0) n = 1;
        if (n <= capacity) return;
        capacity = n;

        T* old = data;
        allocate();
        memcpy(data, old, size * sizeof(T));
        ::operator delete(old);
    }

    void allocate() { data = (T*)(::operator new(capacity * sizeof(T))); }

    void callConstructors(u64 i = 0) {
        for (; i < size; i++) std::construct_at(data + i);
    }

    void callDestructors(u64 i = 0) {
        for (; i < size; i++) std::destroy_at(data + i);
    }
};

template <typename T>
struct std::formatter<Vec<T>> : std::formatter<T> {
    auto format(const Vec<T>& v, auto& ctx) const {
        std::string s;
        for (u64 i = 0; i < v.getSize(); i++) {
            s += std::format("{}", v[i]);
            if (i != v.getSize() - 1) s += ", ";
        }

        return std::format_to(ctx.out(), "{{{}}}", s);
    }
};