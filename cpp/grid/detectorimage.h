//
// Created by kvik on 24/01/24.
//

#ifndef ANASTASIS_CPP_DETECTORIMAGE_H
#define ANASTASIS_CPP_DETECTORIMAGE_H

#include "grid/grid.h"


/**
 * Extends an abstract grid with actual pixel data. Implemented in separate structures for cache speed
 */
class DetectorImage: public Grid {
private:
    std::vector<std::vector<Pixel>> _pixels;
    Matrix _data;
public:
    DetectorImage(Point centre, std::pair<std::size_t, std::size_t> grid_size, std::pair<real, real> physical_size,
                  real rotation, std::pair<real, real> pixfrac);
    DetectorImage(Point centre, std::pair<real, real> physical_size, real rotation, std::pair<real, real> pixfrac,
                  const Matrix & data);

    real operator[](int row, int col) const;
    real & operator[](int row, int col);
};


#endif //ANASTASIS_CPP_DETECTORIMAGE_H
