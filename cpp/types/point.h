#ifndef ANASTASIS_CPP_POINT_H
#define ANASTASIS_CPP_POINT_H

#include <cmath>

#include "types.h"

class Point {
public:
    real x {0};
    real y {0};
    Point rotated(real angle);

    Point & operator+=(Point shift);
    Point & operator-=(Point shift);
    Point & operator*=(real scale);
    Point & operator/=(real scale);
};

inline real distance_euclidean(const Point p1, const Point p2) {
    real dx = p2.x - p1.x;
    real dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
};

inline real dot(const Point p1, const Point p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

inline real cross(const Point p1, const Point p2) {
    return p1.x * p2.y - p2.x * p1.y;
}

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

static Point segment_intersection(Point p0, Point p1, Point q0, Point q1);
static Point segment_intersection_nondeg(Point p, Point q, Point b);

#endif //ANASTASIS_CPP_POINT_H
