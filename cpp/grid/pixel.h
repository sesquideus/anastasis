#ifndef ANASTASIS_CPP_PIXEL_H
#define ANASTASIS_CPP_PIXEL_H

#include <array>
#include "../types/point.h"

class Pixel {
private:
    std::array<std::array<Point, 2>, 2> corners_;

public:
    explicit Pixel();
    explicit Pixel(Point bottomleft, Point bottomright, Point topleft, Point topright);

    inline std::array<std::array<Point, 2>, 2> corners() const { return this->corners_; };
    Pixel bounding_box(real slack) const;
    void print() const;
};


#endif //ANASTASIS_CPP_PIXEL_H
