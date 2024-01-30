#include <cstddef>
#include <istream>
#include <fstream>
#include <exception>

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

/**
 * Load a detector image from a BMP file (quick and easy, minimal error checking, requires 8bpp greyscale)
 * @param centre            Position of the centre of the image in world coordinates
 * @param physical_size     Physical dimensions of the image (w × h)
 * @param rotation          Rotation of the image (radians)
 * @param pixfrac           Fill factor of pixels (horizontal, vertical)
 * @param filename          Filename to load the pixels from (8 bit BMP)
 */
DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                             const std::string & filename):
                             Grid(centre, {1536, 2048}, physical_size, rotation, pixfrac) {
    std::ifstream image;
    image.open(filename);
    unsigned short header, planes, bpp;
    unsigned char value;
    int buf;
    int offset, width, height;
    image.read((char *) &header, 2);
    if (header != 0x4D42) {
        throw std::runtime_error("Invalid BMP magic value");
    }
    fmt::print("Header is ok\n");

    image.seekg(10, std::ios::beg);
    image.read((char *) &offset, 4);
    image.seekg(4, std::ios::cur);
    image.read((char *) &width, 4);
    image.read((char *) &height, 4);
    image.read((char *) &planes, 2);
    if (planes != 0x0001) {
        throw std::runtime_error(fmt::format("Invalid number of image planes {}, must be 1", planes));
    }
    image.read((char *) &bpp, 2);
    if (bpp != 0x0008) {
        throw std::runtime_error(fmt::format("Invalid BPP {}, must be 8", bpp));
    }
    image.seekg(offset, std::ios::beg);
    
    this->_data.resize(width, height);
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            image.read((char *) &value, 1);
            this->_data(col, row) = static_cast<real>(value) / 256.0;
        }
    }
    image.close();
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

