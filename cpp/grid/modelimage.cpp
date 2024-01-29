#include "modelimage.h"

ModelImage::ModelImage(int width, int height):
    width_(width),
    height_(height),
    data_(width, height)
{}

Pixel ModelImage::pixel(int x, int y) const {
    if ((x < 0) || (x >= this->width_) || (y < 0) || (y >= this->height_)) {
        return Pixel::invalid();
    } else {
        real left = static_cast<real>(x);
        real right = static_cast<real>(x) + 1.0;
        real bottom = static_cast<real>(y);
        real top = static_cast<real>(y) + 1.0;
        return Pixel(Point(left, bottom), Point(right, bottom), Point(left, top), Point(right, top));
    }
}

void ModelImage::drizzle(const std::vector<DetectorImage> & images) {
    /** Drizzle a vector of DetectorImages onto this ModelImage **/
    for (auto && image: images) {
        *this += image;
    }
}

void ModelImage::operator+=(const DetectorImage & image) {
    /** Drizzle a DetectorImage into this ModelImage **/
    int total = 0;
    int inspected = 0;
    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            // Find the orthogonal bounds of the pixel so that many unnecessary computations can be avoided completely
            Box bounds = image.world_pixel(row, col).bounding_box();
            Pixel pixel = this->pixel(row, col);

            for (int y = bounds.bottom(); y < bounds.top(); ++y) {
                if ((y < 0) || (y >= image.height())) {
                    continue;
                }
                for (int x = bounds.left(); x < bounds.right(); ++x) {
                    if ((x < 0) || (x >= image.width())) {
                        continue;
                    }
                    inspected++;
                    real overlap = pixel & image.world_pixel(x, y);
                   // fmt::print("{}\n{}\n{}\n", pixel, image.world_pixel(x, y), overlap);
                    if (overlap > Grid::NegligibleOverlap) ++total;
                    this->data_(row, col) += overlap * image[x, y];
                }
            }
            //fmt::print("\n\n");
        }
    }
    fmt::print("{} {}\n", total, inspected);
}