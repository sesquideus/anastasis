#ifndef ANASTASIS_CPP_PIXEL_H
#define ANASTASIS_CPP_PIXEL_H

#include <array>
#include "../types/point.h"

class Pixel {
private:
    std::array<std::array<Point, 2>, 2> corners_;
    constexpr static real SLACK = 1e-12;
public:
    explicit Pixel();
    explicit Pixel(Point bottomleft, Point bottomright, Point topleft, Point topright);

    inline Point a() const { return this->corners_[0][0]; }
    inline Point b() const { return this->corners_[0][1]; }
    inline Point c() const { return this->corners_[1][1]; }
    inline Point d() const { return this->corners_[1][0]; }

    [[nodiscard]] inline std::array<std::array<Point, 2>, 2> corners() const { return this->corners_; };
    [[nodiscard]] Pixel bounding_box(real slack = 1e-6) const;
    void print() const;
    [[nodiscard]] bool contains(Point point) const;

    [[nodiscard]] real area() const;
    [[nodiscard]] real overlap(const Pixel & other) const; // Area of overlap
};


#endif //ANASTASIS_CPP_PIXEL_H
