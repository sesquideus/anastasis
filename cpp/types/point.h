#ifndef ANASTASIS_CPP_POINT_H
#define ANASTASIS_CPP_POINT_H

#include <cmath>

#include "types.h"

typedef struct Point {
    real x {0};
    real y {0};
} Point;

inline real distance_euclidean(const Point p1, const Point p2) {
    real dx = p2.x - p1.x;
    real dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
};

/*
template<int D>
class PointX {
private:
    real x;
    real y;
public:
    inline real mag2() { return this->x * this->x + this->y * this->y; }
    inline real distance_to(const Point other, const MetricFunctor<Point> & metric) const { metric(*this, other); }
};*/

#endif //ANASTASIS_CPP_POINT_H
