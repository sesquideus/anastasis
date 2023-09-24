#ifndef ANASTASIS_CPP_METRICS_H
#define ANASTASIS_CPP_METRICS_H

#include "../types/types.h"
#include "../types/point.h"

template<class T>
class MetricFunctor {
public:
    virtual inline real operator()(const T & first, const T & second) const = 0;
};

class Euclidean: public MetricFunctor<Point> {
public:
    inline real operator()(const Point & first, const Point & second) const override {
        real dx = second.x - first.x;
        real dy = second.y - first.y;
        return std::sqrt(dx * dx + dy * dy);
    };
};

class Manhattan: public MetricFunctor<Point> {
public:
    inline real operator()(const Point first, const Point second) const {
        return std::fabs(second.x - first.x) + std::fabs(second.y - first.y);
    };
};

class Maximum: public MetricFunctor<Point> {
public:
    inline real operator()(const Point first, const Point second) const {
        return std::max(std::fabs(second.x - first.x), std::fabs(second.y - first.y));
    };
};

#endif //ANASTASIS_CPP_METRICS_H
