#include <cstddef>
#include "spectralgrid.h"


DetectorImage::DetectorImage(Point centre, std::pair<std::size_t, std::size_t> grid_size,
                             std::pair<real, real> physical_size, real rotation, std::pair<real, real> pixfrac):
                             Grid(centre, grid_size, physical_size, rotation, pixfrac) {
    this->_data.resize(grid_size.first, grid_size.second);
}

DetectorImage::DetectorImage(Point centre, std::pair<real, real> physical_size, real rotation,
                             std::pair<real, real> pixfrac, const Matrix & data):
                             Grid(centre, {data.rows(), data.cols()}, physical_size, rotation, pixfrac) {
    this->_data = data;
}
