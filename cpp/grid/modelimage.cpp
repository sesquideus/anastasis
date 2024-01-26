#include "modelimage.h"

ModelImage::ModelImage(int width, int height):
    width_(width),
    height_(height)
{}

std::vector<std::vector<Pixel>> ModelImage::coordinates() const {
    std::vector<std::vector<Pixel>> output(this->height_, std::vector<Pixel>(this->width_));
    return output;
}

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
    for (auto && image: images) {
        *this += image;
    }
}

void ModelImage::operator+=(const DetectorImage & image) {
    /** Drizzle a DetectorImage into this ModelImage **/
    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            // Find the orthogonal bounds of the pixel so that many unnecessary computations can be avoided
            Box bounds = image.world_pixel(row, col).bounding_box();
            Pixel pixel = this->pixel(col, row);

            for (int y = bounds.bottom(); y <= bounds.top(); ++y) {
                for (int x = bounds.left(); x <= bounds.right(); ++x) {
                    this->data_(row, col) += pixel.overlap(this->pixel(x, y)) * image[x, y];
                }
            }
        }
    }
}