#ifndef SPATIAL_H
#define SPATIAL_H

#include <cmath>
#include <vector>

#include "../types/types.h"
#include "metrics.h"
#include "distant.h"

template<class M, class T>
concept IsMetric = requires(M m, T a, T b) {
    { m(a, b) } -> std::same_as<real>;
};

template<class T, class M>
requires IsMetric<M, T>
class Spatial {
private:
    unsigned long int size_ {0};
public:
    virtual ~Spatial() = 0;

    virtual const Distant<T> & find_nearest(const T & point) const = 0;
    virtual std::vector<Distant<T>> find_nearest(const T & point, unsigned int k) const = 0;
    virtual std::vector<Distant<T>> find_radius(const T & point, real radius) const = 0;

    virtual void print() const = 0;
};

#endif // SPATIAL_H
