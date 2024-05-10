#define FMT_HEADER_ONLY
#include <fmt/format.h>

#include <chrono>
#include "types/types.h"


real trim(real value, real lower = 0.0, real upper = 1.0) {
    return (value < lower) ? lower : ((value > upper) ? upper : value);
}

void time_function(const std::function<real(void)> & f) {
    auto start = std::chrono::high_resolution_clock::now();
    fmt::print("Result is {}\n", f());
    auto diff = std::chrono::high_resolution_clock::now() - start;
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(diff);

    fmt::print("Test took {} \u03BCs\n", us.count());
}
