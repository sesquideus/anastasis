#ifndef SPATIAL_H
#define SPATIAL_H

#include <cmath>
#include <vector>

#include "../types/types.h"
#include "metrics.h"
#include "distant.h"

namespace Astar {
    template<class T, class Metric> requires IsMetric<T, Metric> class Spatial {
    protected:
        Metric metric_;
        unsigned long int size_ {0};
    public:
        explicit Spatial();
        virtual ~Spatial() = 0;

        virtual const Distant<T> & find_nearest(const T & point) const = 0;
        virtual std::vector<Distant<T>> find_nearest(const T & point, unsigned int k) const = 0;
        virtual std::vector<Distant<T>> find_radius(const T & point, real radius) const = 0;

        virtual void print() const = 0;
    };

    template <class T, class Metric> requires IsMetric<T, Metric>
    Spatial<T, Metric>::Spatial():
        metric_() {}
}

#endif // SPATIAL_H
