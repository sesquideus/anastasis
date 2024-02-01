#ifndef ANASTASIS_CPP_DETECTORIMAGE_H
#define ANASTASIS_CPP_DETECTORIMAGE_H

#include <random>
#include "grid/grid.h"


/**
 * Extends an abstract grid with actual pixel data. Implemented in separate structures to enable vectorization
 */
class DetectorImage: public Grid {
private:
    Matrix _data;
    static pair<int> read_bitmap(const std::string & filename);
public:
    DetectorImage(Point centre, pair<std::size_t> grid_size, pair<real> physical_size,
                  real rotation, pair<real> pixfrac);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const Matrix & data);
    DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                  const std::string & filename);

    inline real operator[](int col, int row) const {
        return this->_data(col, row);
    }
    inline real & operator[](int col, int row) {
        return this->_data(col, row);
    }

    DetectorImage & apply(const std::function<real(void)> & function);
    DetectorImage & apply(const std::function<real(real)> & function);

    DetectorImage & multiply(real value);

    void fill(real value = 0.0);

    void randomize();
};


#endif //ANASTASIS_CPP_DETECTORIMAGE_H
