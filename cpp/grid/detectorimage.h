#ifndef ANASTASIS_CPP_DETECTORIMAGE_H
#define ANASTASIS_CPP_DETECTORIMAGE_H

#include <random>
#include "grid/grid.h"


/**
 * Extends an abstract grid with actual pixel data. Implemented in separate structures for cache speed
 */
class DetectorImage: public Grid {
private:
    Matrix _data;
public:
    DetectorImage(Point centre, pair<std::size_t> grid_size, pair<real> physical_size,
                  real rotation, pair<real> pixfrac);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const Matrix & data);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const std::string & filename);

    inline real operator[](int row, int col) const {
        return this->_data(row, col);
    }
    inline real & operator[](int row, int col) {
        return this->_data(row, col);
    }

    void fill(real value = 0.0);
    void randomize();
};


#endif //ANASTASIS_CPP_DETECTORIMAGE_H
