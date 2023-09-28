#include "point.h"

Point Point::rotated(real angle) const {
    real sina = std::sin(angle);
    real cosa = std::cos(angle);
    return Point(cosa * this->x - sina * this->y, sina * this->x + cosa * this->y);
}

real Point::slope() const {
    return std::atan2(this->y, this->x);
}

Point Point::operator-() const {
    return Point(-this->x, -this->y);
}

Point & Point::operator+=(Point shift) {
    this->x += shift.x;
    this->y += shift.y;
    return *this;
}

Point & Point::operator-=(Point shift) {
    this->x -= shift.x;
    this->y -= shift.y;
    return *this;
}

Point & Point::operator*=(real scale) {
    this->x *= scale;
    this->y *= scale;
    return *this;
}

Point & Point::operator/=(real scale) {
    this->x /= scale;
    this->y /= scale;
    return *this;
}

Point Point::line_segment_intersection(const Point p0, const Point p1, const Point q0, const Point q1) {
    auto p = p1 - p0;
    auto q = q1 - q0;
    auto b = q0 - p0;
    real det = p ^ q;
    if (std::abs(det) < 1e-15) {
        return Point::invalid();
    } else {
        real t = (b ^ q) / det;
        real u = (b ^ p) / det;
        if ((-SLACK <= t) && (t <= 1.0 + SLACK) && (-SLACK <= u) && (u <= 1.0 + SLACK)) {
            return p0 + t * p;
        } else {
            return Point::invalid();
        }
    }
}

