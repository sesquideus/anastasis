#include <cstddef>
#include "detectorimage.h"


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

real DetectorImage::operator[](int row, int col) const {
    return this->_data(row, col);
}

real & DetectorImage::operator[](int row, int col) {
    return this->_data(row, col);
}

void DetectorImage::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::weibull_distribution<> weibull;

    for (unsigned int row = 0; row < this->height(); ++row) {
        for (unsigned int col = 0; col < this->width(); ++col) {
            (*this)[row, col] = weibull(gen);
        }
    }
}
