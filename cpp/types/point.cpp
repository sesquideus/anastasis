#include "point.h"

Point Point::rotated(real angle) {
    real sina = std::sin(angle);
    real cosa = std::cos(angle);
    return Point(cosa * this->x - sina * this->y, sina * this->x + cosa * this->y);
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

