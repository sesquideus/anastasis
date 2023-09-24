#ifndef ANASTASIS_CPP_DISTANT_H
#define ANASTASIS_CPP_DISTANT_H

#include "../types/types.h"
#include "metrics.h"

template<typename T>
class Distant {
private:
    const T & point_;
    real distance_;
public:
    Distant(const T point, real distance):
        point_(point),
        distance_(distance)
    {}

    inline bool operator<(const Distant & other) const {
        return this->distance < other.distance;
    }

    inline bool operator==(const Distant & other) const {
        return (this->point == other.point) && ((this->distance - other.distance) < 1e-12);
    }
};

template<typename T>
class DistantMetricFunctor {
private:
    const MetricFunctor<T> & metric_;
public:
    DistantMetricFunctor(const MetricFunctor<T> & metric): metric_(metric) {}

    inline real operator()(Distant<T> a, Distant<T> b) const {
        return this->metric_(a.point, b.point);
    }
};

#endif //ANASTASIS_CPP_DISTANT_H
