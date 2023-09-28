#include "canonicalgrid.h"

CanonicalGrid::CanonicalGrid(unsigned int width, unsigned int height):
    width_(width),
    height_(height)
{}

std::vector<std::vector<Pixel>> CanonicalGrid::coordinates() {
    std::vector<std::vector<Pixel>> output(this->height_, std::vector<Pixel>(this->width_));
    return output;

}

Pixel CanonicalGrid::pixel(unsigned int x, unsigned int y) const {
    real left = static_cast<real>(x);
    real right = static_cast<real>(x) + 1.0;
    real bottom = static_cast<real>(y);
    real top = static_cast<real>(y) + 1.0;
    return Pixel(Point(left, bottom), Point(right, bottom), Point(left, top), Point(right, top));
}
