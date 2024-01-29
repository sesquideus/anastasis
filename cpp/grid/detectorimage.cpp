#include <cstddef>
#include "detectorimage.h"


DetectorImage::DetectorImage(Point centre, pair<std::size_t> grid_size,
                             std::pair<real, real> physical_size, real rotation, std::pair<real, real> pixfrac):
                             Grid(centre, grid_size, physical_size, rotation, pixfrac) {
    this->_data.resize(grid_size.first, grid_size.second);
    this->_data.setZero();
}

DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation,
                             pair<real> pixfrac, const Matrix & data):
                             Grid(centre, {data.rows(), data.cols()}, physical_size, rotation, pixfrac) {
    this->_data = data;
}

real DetectorImage::operator[](int row, int col) const {
    return this->_data(row, col);
}

real & DetectorImage::operator[](int row, int col) {
    return this->_data(row, col);
}

void DetectorImage::fill(const real value) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[row, col] = value;
        }
    }
}

void DetectorImage::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::weibull_distribution<> weibull;

    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[row, col] = weibull(gen);
        }
    }
}
