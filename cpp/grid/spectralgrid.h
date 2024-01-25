//
// Created by kvik on 24/01/24.
//

#ifndef ANASTASIS_CPP_SPECTRALGRID_H
#define ANASTASIS_CPP_SPECTRALGRID_H

#include "grid.h"


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
};


//class SpectralDetectorImage: public DetectorImage {
//private:
//    std::vector
//public:
//    void calibrate_wavelengths(const WavelengthCalibrator & calibrator);
//
//};

#endif //ANASTASIS_CPP_SPECTRALGRID_H
