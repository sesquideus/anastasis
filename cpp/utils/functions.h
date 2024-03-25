#ifndef ANASTASIS_CPP_FUNCTIONS_H
#define ANASTASIS_CPP_FUNCTIONS_H

#include <vector>
#include <iterator>
#include <numeric>
#include <functional>

#include "types/types.h"

template<class T, class Comp>
std::vector<std::size_t> sort_permutation(const std::vector<T> & vec, Comp & comparator) {
    std::vector<std::size_t> perm(vec.size());
    std::iota(perm.begin(), perm.end(), 0);
    std::sort(perm.begin(), perm.end(),
              [&](std::size_t i, std::size_t j) { return comparator(vec[i], vec[j]); }
    );
    return perm;
}

template<class T, class U>
std::vector<T> & sort_by(std::vector<T> & vec, const std::vector<U> & ordering) {
    auto perm = sort_permutation(ordering);
    return permutate_in_place(vec, perm);
}

template<typename T>
std::vector<T> & permutate_in_place(std::vector<T> & vec, const std::vector<std::size_t> & perm) {
    /**
     * Permutate a vector in place by a permutation vector
     * Guaranteed in linear time
     */
    std::vector<bool> done(vec.size(), false);
    for (std::size_t i = 0; i < vec.size(); ++i) {
        if (done[i]) {
            continue;
        }

        done[i] = true;
        std::size_t prev = i;
        std::size_t j = perm[i];
        while (i != j) {
            std::swap(vec[prev], vec[j]);
            done[j] = true;
            prev = j;
            j = perm[j];
        }
    }
    return vec;
}

template<class T>
std::vector<T> flatten(const std::vector<std::vector<T>> & twod) {
    std::vector<T> output;
    for (auto const & inner: twod) {
        output.insert(output.end(), inner.begin(), inner.end());
    }
    return output;
}

template<class T, class U>
std::vector<U> map(const std::vector<T> & vec, std::function<U(T)> f) {
    /**
     * Map a function f: T -> U to every element of vec<T>
     * Returns: vector<U>
     */
    std::vector<U> out;
    out.reserve(vec.size());
    return std::transform(vec.begin(), vec.end(), std::back_inserter(out), f);
}

template<class T>
std::vector<T> & for_each(std::vector<T> & vec, std::function<void(T)> f) {
    /**
     * Shorthand for map
     */
    std::for_each(vec.begin(), vec.end(), f);
}

real trim(real value, real lower, real upper);
void time_function(const std::function<real(void)> & f);

#endif //ANASTASIS_CPP_FUNCTIONS_H
