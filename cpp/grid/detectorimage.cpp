#include <istream>

#include "abstractgrid.h"
#include "detectorimage.h"


DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                             pair<int> grid_size):
        AbstractGrid(grid_size.first, grid_size.second),
        PlacedGrid(centre, grid_size, physical_size, rotation, pixfrac),
        Image(grid_size.first, grid_size.second)
{}

DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                             const Matrix & data):
        AbstractGrid(data.cols(), data.rows()),
        PlacedGrid(centre, {data.cols(), data.rows()}, physical_size, rotation, pixfrac),
        Image(data)
{
    // fmt::println("Created a DetectorImage with size {}×{}, physical size {}", data.cols(), data.rows(), physical_size);
}

/**
 * Load a detector image from a BMP file (quick and easy, only minimal error checking, requires 8bpp greyscale)
 * @param centre            Position of the centre of the image in world coordinates
 * @param physical_size     Physical dimensions of the image (w × h)
 * @param rotation          Rotation of the image (radians)
 * @param pixfrac           Fill factor of pixels (horizontal, vertical)
 * @param filename          Filename to load the pixels from (8 bit BMP)
 */
DetectorImage::DetectorImage(Point centre, pair<real> physical_size, real rotation, pair<real> pixfrac,
                             const std::string & filename):
        AbstractGrid(read_bitmap_header(filename)),
        PlacedGrid(centre, read_bitmap_header(filename), physical_size, rotation, pixfrac),
        Image(filename)
{ }

void DetectorImage::fill(const real value) {
    this->map_in_place([=](real unused) { return value; });
}

/** Apply a parameterless function to every pixel (constant, random, ...) **/
DetectorImage & DetectorImage::map_in_place(const std::function<real()> & function) {
    Image::map_in_place(function);
    return *this;
}

/** Apply a parameterless function to every pixel (constant, random, ...) **/
DetectorImage & DetectorImage::map_in_place(const std::function<real(real)> & function) {
    Image::map_in_place(function);
    return *this;
}

/** Apply a R -> R function to every pixel depending on pixel coordinates and current value **/
DetectorImage & DetectorImage::map_in_place(const std::function<real(int, int, real)> & function) {
    Image::map_in_place(function);
    return *this;
}

void DetectorImage::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::weibull_distribution<> weibull;

    this->map_in_place([&]() { return weibull(gen); });
}

DetectorImage & DetectorImage::multiply(const real value) {
    return this->map_in_place([value](real x) { return value * x; });
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
