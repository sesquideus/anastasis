#ifndef ANASTASIS_CPP_CANONICALGRID_H
#define ANASTASIS_CPP_CANONICALGRID_H

#include <vector>
#include "pixel.h"

class CanonicalGrid {
private:
    int width_;
    int height_;
public:
    explicit CanonicalGrid(int width, int height);

    static Pixel pixel(unsigned int x, unsigned int y) ;
    [[nodiscard]] std::vector<std::vector<Pixel>> coordinates() const;

    inline int width() const { return this->width_; }
    inline int height() const { return this->height_; }
    inline int size() const { return this->width_ * this->height_; }
};

#endif //ANASTASIS_CPP_CANONICALGRID_H
