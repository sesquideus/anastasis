#ifndef ANASTASIS_CPP_DETECTORIMAGE_H
#define ANASTASIS_CPP_DETECTORIMAGE_H

#include <random>
#include "grid/grid.h"


/**
 * Extends an abstract grid with actual pixel data. Implemented in separate structures for cache speed
 */
class DetectorImage: public Grid {
private:
    std::vector<std::vector<Pixel>> _pixels;
    Matrix _data;
public:
    DetectorImage(Point centre, std::pair<std::size_t, std::size_t> grid_size, pair<real> physical_size,
                  real rotation, pair<real> pixfrac);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const Matrix & data);

    real operator[](int row, int col) const;
    real & operator[](int row, int col);

    void fill(const real value = 0.0);
    void randomize();
};


#endif //ANASTASIS_CPP_DETECTORIMAGE_H
