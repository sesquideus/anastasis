#include "modelimage.h"

ModelImage::ModelImage(int width, int height):
    width_(width),
    height_(height),
    data_(width, height)
{
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            this->data_(col, row) = 0;
        }
    }
}

Pixel ModelImage::pixel(int x, int y) const {
    if ((x < 0) || (x >= this->width_) || (y < 0) || (y >= this->height_)) {
        return Pixel::invalid();
    } else {
        real left = static_cast<real>(x);
        real right = left + 1.0;
        real bottom = static_cast<real>(y);
        real top = bottom + 1.0;
        return Pixel(
            Point(left, bottom),
            Point(right, bottom),
            Point(left, top),
            Point(right, top)
        );
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
    real coverage = 0;
    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            // Find the orthogonal bounds of the pixel so that many unnecessary computations can be avoided completely
            const Pixel & image_pixel = image.world_pixel(col, row);
            Box bounds = image_pixel.bounding_box(0);

            for (int y = bounds.bottom(); y < bounds.top(); ++y) {
                if ((y < 0) || (y >= this->height())) {
                    continue;
                }
                for (int x = bounds.left(); x < bounds.right(); ++x) {
                    if ((x < 0) || (x >= this->width())) {
                        continue;
                    }
                    auto && model_pixel = this->pixel(x, y);
                    real overlap = model_pixel & image_pixel;
                    //fmt::print("{}\n{}\n", model_pixel, overlap);
                    if (overlap > Grid::NegligibleOverlap) {
                      //  fmt::print("Overlap {}\n", overlap);
                        ++total;
                    }
                    this->data_(col, row) += overlap * image[x, y];

                    inspected++;
                }
            }
        }
    }
    fmt::print("{} {} {:f}\n", total, inspected, this->total_flux());
}

char character(real what) {
    if (what < 0.01) return ' ';
    if (what < 0.2) return '.';
    if (what < 0.4) return ',';
    if (what < 0.6) return 'o';
    if (what < 0.8) return 'O';
    return 'G';
}

real ModelImage::total_flux() const {
    real out = 0.0;
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            fmt::print("{}", character(this->data_(col, row)));
            out += this->data_(col, row);
        }
        fmt::print("\n");
    }
    return out;
}
