#pragma once

#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>

#include "Ptr.hpp"
#include "String.hpp"
#include "Types.hpp"
#include "Vec.hpp"

template <typename... Args>
void print(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::vformat(fmt.get(), std::make_format_args(args...));
}

template <typename... Args>
void println(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::vformat(fmt.get(), std::make_format_args(args...)) << std::endl;
}