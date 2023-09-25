#ifndef ANASTASIS_CPP_CANONICALGRID_H
#define ANASTASIS_CPP_CANONICALGRID_H

#include <vector>
#include "pixel.h"

class CanonicalGrid {
private:
    unsigned int width_;
    unsigned int height_;
public:
    explicit CanonicalGrid(unsigned int width, unsigned int height);

    Pixel pixel(unsigned int x, unsigned int y);
    std::vector<std::vector<Pixel>> coordinates();
};

#endif //ANASTASIS_CPP_CANONICALGRID_H
