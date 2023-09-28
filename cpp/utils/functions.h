//
// Created by kvik on 9/28/23.
//

#ifndef ANASTASIS_CPP_FUNCTIONS_H
#define ANASTASIS_CPP_FUNCTIONS_H

#include <vector>
#include <iterator>
#include <numeric>
#include <functional>


template<class T, class Comp>
std::vector<std::size_t> sort_permutation(const std::vector<T> & vec, Comp & comparator) {
    std::vector<std::size_t> perm(vec.size());
    std::iota(perm.begin(), perm.end(), 0);
    std::sort(perm.begin(), perm.end(), [&](std::size_t i, std::size_t j) { return comparator(vec[i], vec[j]); });
    return perm;
}

template<class T, class U>
std::vector<T> & sort_by(std::vector<T> & vec, const std::vector<U> & ordering) {
    auto perm = sort_permutation(ordering);
    permutate_in_place(vec, perm);
}


template<typename T>
std::vector<T> & permutate_in_place(std::vector<T> & vec, const std::vector<std::size_t> & perm) {
    std::vector<bool> done(vec.size(), false);
    for (std::size_t i = 0; i < vec.size(); ++i) {
        if (done[i]) continue;

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


template<class T, class U>
std::vector<U> map(const std::vector<T> & vec, std::function<U(T)> f) {
    std::vector<U> out;
    out.reserve(vec.size());
    return std::transform(vec.begin(), vec.end(), std::back_inserter(out), f);
}

template<class T>
std::vector<T> & for_each(std::vector<T> & vec, std::function<T(T)> f) {
    std::for_each(vec.begin(), vec.end(), f);
}


#endif //ANASTASIS_CPP_FUNCTIONS_H
