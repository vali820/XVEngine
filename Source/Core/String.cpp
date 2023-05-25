#include "String.hpp"

#include <cstring>

String::String() : size(0), capacity(0) { allocate(); }

String::~String() { delete[] data; }

String::String(const char *s) {
    size     = strlen(s);
    capacity = size;
    allocate();
    memcpy(data, s, size);
}

String::String(u64 n, char x) : size(n), capacity(n) {
    allocate();
    memset(data, x, size);
}

String::String(const String &s) : size(s.size), capacity(s.size) {
    allocate();
    memcpy(data, s.data, size);
}

String::String(String &&s) noexcept : size(s.size), capacity(s.capacity), data(s.data) {
    s.data     = nullptr;
    s.size     = 0;
    s.capacity = 0;
}

String &String::operator=(const String &s) {
    if (this == &s) return *this;

    delete[] data;
    data = nullptr;

    size     = s.size;
    capacity = s.size;
    allocate();
    memcpy(data, s.data, size);

    return *this;
}

String &String::operator=(String &&s) noexcept {
    delete[] data;
    data     = s.data;
    size     = s.size;
    capacity = s.capacity;

    s.data     = nullptr;
    s.size     = 0;
    s.capacity = 0;
    return *this;
}

void String::push(char c) {
    if (size >= capacity) reallocate(capacity * 2);
    data[size]     = c;
    data[size + 1] = '\0';
}

void String::push(const char *s) {
    u64 sSize = strlen(s);
    while (size + sSize >= capacity) reallocate(capacity * 2);
    memcpy(data + size, s, sSize);
    size       += sSize;
    data[size]  = '\0';
}

void String::push(const String &s) {
    while (size + s.size >= capacity) reallocate(capacity * 2);
    memcpy(data + size, s.data, s.size);
    size       += s.size;
    data[size]  = '\0';
}

void String::remove(u64 index) {
    if (index >= size) return;

    memmove(data + index, data + index + 1, size - index);  // Move one extra byte for terminating 0
    size--;
}

void String::remove(u64 begin, u64 end) {
    if (begin >= size or begin >= end) return;
    if (end > size) end = size;
    u64 removed = end - begin;
    memmove(data + begin, data + end, size - end + 1);
    size -= removed;
}

void String::clear() {
    size    = 0;
    data[0] = '\0';
}

void String::resize(u64 n) {
    if (n == size) return;
    if (n > size) {
        reallocate(n);
    }
    size = n;
}

void String::allocate() {
    data       = new char[capacity + 1];
    data[size] = '\0';
}

void String::reallocate(u64 n) {
    if (n <= capacity) return;
    capacity = n;

    char *old = data;
    allocate();
    memcpy(data, old, size);
    delete[] old;
}