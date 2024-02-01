#include <ostream>
#include <fstream>

#include "modelimage.h"
#include "utils/functions.h"

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

ModelImage & ModelImage::operator+=(const DetectorImage & image) {
    /** Drizzle a DetectorImage into this ModelImage **/
    int total = 0;
    int inspected = 0;

    // For every pixel of the drizzling image
    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            // Find the orthogonal bounds of the pixel so that many unnecessary computations can be avoided completely
            const Pixel & image_pixel = image.world_pixel(col, row);
            Box bounds = image_pixel.bounding_box(0);

            // For every pixel caught in the drizzle
            for (int y = bounds.bottom; y < bounds.top; ++y) {
                if ((y < 0) || (y >= this->height())) {
                    continue;
                }
                for (int x = bounds.left; x < bounds.right; ++x) {
                    if ((x < 0) || (x >= this->width())) {
                        continue;
                    }
                    auto && model_pixel = this->pixel(x, y);

                    real overlap = model_pixel & image_pixel;
                    if (overlap > Grid::NegligibleOverlap) {
                        ++total;
                    }
                    (*this)[x, y] += overlap * image[col, row] / image.pixel_area(col, row);

                    inspected++;
                }
            }
        }
    }
    return *this;
}

char character(real what) {
    if (what < 0.1) return ' ';
    if (what < 0.2) return '.';
    if (what < 0.3) return ',';
    if (what < 0.4) return '-';
    if (what < 0.5) return '=';
    if (what < 0.6) return 'o';
    if (what < 0.7) return 'O';
    if (what < 0.8) return '8';
    if (what < 0.9) return '%';
    return '@';
}

real ModelImage::total_flux() const {
    real out = 0.0;
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            char c = character(this->data_(col, row));
            fmt::print("{}{}{}", c, c, c);
            out += this->data_(col, row);
        }
        fmt::print("\n");
    }
    return out;
}

void ModelImage::save(const std::string & filename) const {
    std::ofstream out;
    out.open(filename);
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            //real value = trim((*this)[col, row], 0, 1);
            real value = (*this)[col, row];
            out.write(reinterpret_cast<const char*>(&value), sizeof value);
        }
    }
    out.close();
}

real ModelImage::operator^(const ModelImage & other) const {
    real diff = 0;
    real this_sq = 0;
    real other_sq = 0;
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            diff += std::pow((*this)[col, row] - other[col, row], 2);
            this_sq += std::pow((*this)[col, row], 2);
            other_sq += std::pow(other[col, row], 2);
        }
    }
    return diff / (this_sq + other_sq);
}