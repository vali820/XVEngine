#pragma once

#include <format>

#include "Core/Iterator.hpp"
#include "Types.hpp"

class String {
   private:
    u64 size;
    u64 capacity;
    char* data = nullptr;

   public:
    String();
    String(const char* s);
    String(u64 n, char x);
    ~String();

    String(const String& s);
    String(String&& s) noexcept;
    String& operator=(const String& s);
    String& operator=(String&& s) noexcept;

    void push(char c);
    void push(const char* s);
    void push(const String& s);

    void remove(u64 index);
    void remove(u64 begin, u64 end);
    void clear();

    void resize(u64 n);

    inline char& operator[](u64 pos) { return data[pos]; }
    inline const char& operator[](u64 pos) const { return data[pos]; }

    [[nodiscard]] inline u64 getSize() const { return size; }
    [[nodiscard]] inline u64 getCapacity() const { return capacity; }
    inline char* getData() { return data; }
    [[nodiscard]] inline const char* getData() const { return data; }

    [[nodiscard]] inline Iterator<char> begin() const { return Iterator(data); }
    [[nodiscard]] inline Iterator<char> end() const { return Iterator(data + size); }

   private:
    void allocate();
    void reallocate(u64 n);
};

template <>
struct std::formatter<String> : std::formatter<char> {
    auto format(const String& s, auto& ctx) const { return std::format_to(ctx.out(), "{}", s.getData()); }
};
