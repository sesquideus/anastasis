#include "canonicalgrid.h"

CanonicalGrid::CanonicalGrid(unsigned int width, unsigned int height):
    width_(width),
    height_(height)
{}

std::vector<std::vector<Pixel>> CanonicalGrid::coordinates() {
    std::vector<std::vector<Pixel>> output(this->height_, std::vector<Pixel>(this->width_));
    return output;

}
