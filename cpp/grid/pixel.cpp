//
// Created by kvik on 9/25/23.
//

#include <fmt/core.h>
#include "pixel.h"

Pixel::Pixel():
        corners_ {{{Point(0, 0), Point(0, 0)}, {Point(0, 0), Point(0, 0)}}}
{}

Pixel::Pixel(const Point bottomleft, const Point bottomright, const Point topleft, const Point topright):
        corners_ {{{bottomleft, bottomright}, {topleft, topright}}}
{ }

void Pixel::print() const {
    fmt::print("Pixel at {:9.6f} {:9.6f} -- {:9.6f} {:9.6f} -- {:9.6f} {:9.6f} -- {:9.6f} {:9.6f}\n",
               this->corners_[0][0].x, this->corners_[0][0].y,
               this->corners_[0][1].x, this->corners_[0][1].y,
               this->corners_[1][0].x, this->corners_[1][0].y,
               this->corners_[1][1].x, this->corners_[1][1].y
    );
}

Pixel Pixel::bounding_box(real slack = 1e-6) const {
    auto xs = {this->corners_[0][0].x, this->corners_[0][1].x, this->corners_[1][0].x, this->corners_[1][1].x};
    auto ys = {this->corners_[0][0].y, this->corners_[0][1].y, this->corners_[1][0].y, this->corners_[1][1].y};
    real minx = std::min(xs) - slack;
    real maxx = std::max(xs) + slack;
    real miny = std::min(ys) - slack;
    real maxy = std::max(ys) + slack;

    return Pixel(Point(minx, miny), Point(minx, maxy), Point(maxx, miny), Point(maxx, maxy));
}
