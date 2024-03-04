#include <cstddef>
#include <istream>
#include <fstream>
#include <exception>

#include "detectorimage.h"


DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                             pair<std::size_t> grid_size):
                             Grid(centre, grid_size, physical_size, rotation, pixfrac) {
    this->_data.resize(grid_size.first, grid_size.second);
    this->_data.setZero();
}

DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                             const Matrix & data):
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
                             Grid(centre, read_bitmap_header(filename), physical_size, rotation, pixfrac)
{
    std::ifstream bitmap_file;
    unsigned short planes, bpp;
    unsigned char value;
    int offset;
    int width = this->width();
    int height = this->height();

    bitmap_file.open(filename);
    bitmap_file.seekg(10, std::ios::beg);
    bitmap_file.read((char *) &offset, 4);
    bitmap_file.seekg(12, std::ios::cur);
    bitmap_file.read((char *) &planes, 2);
    if (planes != 0x0001) {
        throw std::runtime_error(fmt::format("Invalid number of image planes {}, must be 1", planes));
    }
    bitmap_file.read((char *) &bpp, 2);
    if (bpp != 0x0008) {
        throw std::runtime_error(fmt::format("Invalid BPP {}, must be 8", bpp));
    }
    bitmap_file.seekg(offset, std::ios::beg);

    this->_data.resize(width, height);
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            bitmap_file.read((char *) &value, 1);
            this->_data(col, row) = static_cast<real>(value) / 255.0;
        }
    }
    bitmap_file.close();
    fmt::print("Loaded bitmap with size {} × {}\n", width, height);
}

pair<int> DetectorImage::read_bitmap_header(const std::string & filename) {
    std::ifstream bitmap_file;
    unsigned short header;
    int width, height;

    bitmap_file.open(filename);
    if (!bitmap_file.is_open()) {
        throw std::runtime_error(fmt::format("Could not open file {}", filename));
    }
    bitmap_file.seekg(0, std::ios::beg);
    bitmap_file.read((char *) &header, 2);
    if (header != 0x4D42) {
        throw std::runtime_error(fmt::format("Invalid BMP magic value {:04x}", header));
    }
    bitmap_file.seekg(18, std::ios::beg);
    bitmap_file.read((char *) &width, 4);
    bitmap_file.read((char *) &height, 4);
    bitmap_file.close();
    return {width, height};
}

void DetectorImage::fill(const real value) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = value;
        }
    }
}

/** Apply a parameterless function to every pixel (constant, random, ...) **/
DetectorImage & DetectorImage::apply(const std::function<real(void)> & function) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = function();
        }
    }
    return *this;
}

/** Apply a R -> R function to every pixel **/
DetectorImage & DetectorImage::apply(const std::function<real(real)> & function) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = function((*this)[col, row]);
        }
    }
    return *this;
}

/** Apply a R -> R function to every pixel depending on pixel coordinates **/
DetectorImage & DetectorImage::apply(const std::function<real(int, int, real)> & function) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = function(col, row, (*this)[col, row]);
        }
    }
    return *this;
}

void DetectorImage::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::weibull_distribution<> weibull;

    this->apply([&]() { return weibull(gen); });
}

DetectorImage & DetectorImage::multiply(const real value) {
    return this->apply([value](real x) { return value * x; });
}

DetectorImage operator+(const DetectorImage & image, Point shift) {
    auto out = image;
    out += shift;
    return out;
}

DetectorImage operator*(const DetectorImage & image, real scale) {
    auto out = image;
    out *= scale;
    return out;
}

DetectorImage operator*(const DetectorImage & image, pair<real> scale) {
    auto out = image;
    out *= scale;
    return out;
}
