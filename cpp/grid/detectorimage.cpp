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
DetectorImage DetectorImage::load_bitmap(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                                         const std::string & filename) {
    std::ifstream bitmap_file;
    unsigned short header, planes, bpp;
    unsigned char value;
    int offset, width, height;

    bitmap_file.open(filename);
    bitmap_file.read((char *) &header, 2);
    if (header != 0x4D42) {
        throw std::runtime_error("Invalid BMP magic value");
    }
    fmt::print("Header is ok\n");

    bitmap_file.seekg(10, std::ios::beg);
    bitmap_file.read((char *) &offset, 4);
    bitmap_file.seekg(4, std::ios::cur);
    bitmap_file.read((char *) &width, 4);
    bitmap_file.read((char *) &height, 4);
    bitmap_file.read((char *) &planes, 2);
    if (planes != 0x0001) {
        throw std::runtime_error(fmt::format("Invalid number of image planes {}, must be 1", planes));
    }
    bitmap_file.read((char *) &bpp, 2);
    if (bpp != 0x0008) {
        throw std::runtime_error(fmt::format("Invalid BPP {}, must be 8", bpp));
    }
    bitmap_file.seekg(offset, std::ios::beg);

    Matrix matrix(width, height);
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            bitmap_file.read((char *) &value, 1);
            matrix(col, row) = static_cast<real>(value) / 255.0;
        }
    }
    bitmap_file.close();
    fmt::print("Loaded bitmap {} × {}\n", width, height);
    return DetectorImage(centre, physical_size, rotation, pixfrac, matrix);
}


void DetectorImage::fill(const real value) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = value;
        }
    }
}

void DetectorImage::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::weibull_distribution<> weibull;

    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = weibull(gen);
        }
    }
}

