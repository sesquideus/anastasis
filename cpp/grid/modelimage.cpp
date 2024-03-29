#include "modelimage.h"

ModelImage::ModelImage(int width, int height):
        AbstractGrid(width, height),
        Image(width, height),
        variance_(height, width)
{
    this->variance_.setZero();
}

ModelImage::ModelImage(pair<int> size):
    ModelImage(size.first, size.second)
{}

Pixel ModelImage::pixel(int x, int y) const {
    if ((x < 0) || (x >= this->width()) || (y < 0) || (y >= this->height())) {
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

ModelImage & ModelImage::naive_drizzle(const std::vector<DetectorImage> & images) {
    /** Drizzle a vector of DetectorImages onto this ModelImage **/
    for (auto && image: images) {
        this->naive_drizzle(image);
    }

    return *this;
}

ModelImage & ModelImage::weighted_drizzle(const std::vector<DetectorImage> & images) {
    /** Drizzle a vector of DetectorImages onto this ModelImage **/
    for (auto && image: images) {
        this->weighted_drizzle(image);
    }

    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            if (this->variance_(row, col) == 0) {
                (*this)[col, row] = -1;
            } else {
                (*this)[col, row] /= this->variance_(row, col);
            }
        }
    }

    return *this;
}

SparseMatrix ModelImage::overlap_matrix(const DetectorImage & image) const {
    std::vector<Overlap4D> active_pixels;
    // There will be about four times as many overlaps as there are model pixels
    active_pixels.reserve(4 * image.count());

    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            const Pixel & image_pixel = image.world_pixel(col, row);
            Box bounds = image_pixel.bounding_box(0);

            // For every pixel potentially caught in the drizzle
            for (int y = bounds.bottom; y < bounds.top; ++y) {
                if ((y < 0) || (y >= this->height())) {
                    continue;
                }
                for (int x = bounds.left; x < bounds.right; ++x) {
                    if ((x < 0) || (x >= this->width())) {
                        continue;
                    }
                    auto && model_pixel = this->pixel(x, y);

                    // Calculate the overlap of model and data pixels
                    real overlap = model_pixel & image_pixel;
                    active_pixels.emplace_back(col, row, x, y, overlap);
                }
            }
        }
    }
    // Return a row-major matrix of overlaps

    SparseMatrix output(image.height() * image.width(), this->height() * this->width());
    for (auto && pixel: active_pixels) {
        output.insert(
            image.width() * std::get<1>(pixel) + std::get<0>(pixel),
            image.width() * std::get<3>(pixel) + std::get<2>(pixel)
        ) = std::get<4>(pixel);
    }
    output.makeCompressed();
    return output;
}

ModelImage & ModelImage::naive_drizzle(const DetectorImage & image) {
    /** Drizzle a DetectorImage into this ModelImage **/
    int total = 0;
    int inspected = 0;
    real sum = 0;

    // For every pixel of the drizzling image
    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            // Find the orthogonal bounds of the pixel so that many unnecessary computations can be avoided completely
            const Pixel & image_pixel = image.world_pixel(col, row);
            Box bounds = image_pixel.bounding_box(0);

            // For every pixel potentially caught in the drizzle
            for (int y = bounds.bottom; y < bounds.top; ++y) {
                if ((y < 0) || (y >= this->height())) {
                    continue;
                }
                for (int x = bounds.left; x < bounds.right; ++x) {
                    if ((x < 0) || (x >= this->width())) {
                        continue;
                    }
                    auto && model_pixel = this->pixel(x, y);

                    // Calculate the overlap of model and data pixels
                    real overlap = model_pixel & image_pixel;
                    // fmt::print("{} {} {} {}\n", model_pixel, image_pixel, overlap, (*this)[x, y]);
                    if (overlap > PlacedGrid::NegligibleOverlap) {
                        // If not zero or negligibly small, add to the value at [x, y] the value
                        // from source's [col, row], scaled by overlap and pixel area
                        (*this)[x, y] += image[col, row] * overlap / image.pixel_area(col, row);
                        ++total;
                        sum += (*this)[x, y];
                    }
                    // fmt::print("{} {} × {} {} -> {}\n", x, y, row, col, overlap);
                    inspected++;
                }
            }
        }
    }
    return *this;
}

ModelImage & ModelImage::weighted_drizzle(const DetectorImage & image) {
    /** Drizzle a DetectorImage into this ModelImage **/

    // For every pixel of the drizzling image
    for (int row = 0; row < image.height(); ++row) {
        for (int col = 0; col < image.width(); ++col) {
            const Pixel & image_pixel = image.world_pixel(col, row);
            Box bounds = image_pixel.bounding_box(0);

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
                    if (overlap > PlacedGrid::NegligibleOverlap) {
                        // If not zero or negligibly small, add to the value at [x, y] the value
                        // from source's [col, row], scaled by overlap and pixel area
                        this->variance_(y, x) += overlap / image.pixel_area(col, row);
                        (*this)[x, y] += image[col, row] * this->variance_(y, x);
                        if ((3 * row + 2 * col) % 128 == 15) {
                            fmt::print("{} {} - value {} to {} confidence {}\n",
                                       col, row, (*this)[x, y], image[col, row], this->variance_(y, x));
                        }
                    }
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
            char c = character((*this)[col, row]);
            fmt::print("{}{}{}", c, c, c);
            out += (*this)[col, row];
        }
        fmt::print("\n");
    }
    return out;
}

real ModelImage::dot_product(const ModelImage & other) const {
    if (this->size() == other.size()) {
        real diff = 0;
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                diff += (*this)[col, row] * other[col, row];
            }
        }
        return diff;
    } else {
        throw std::invalid_argument(fmt::format("ModelImage sizes do not match_ {} != {}", this->size(), other.size()));
    }
}

real ModelImage::squared_difference(const ModelImage & other) const {
    if (this->size() == other.size()) {
        real diff = 0;
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                diff += std::pow((*this)[col, row] - other[col, row], 2);
            }
        }
        return std::sqrt(diff / static_cast<real>(this->width() * this->height()));
    } else {
        throw std::invalid_argument("ModelImage sizes do not match");
    }
}

real ModelImage::operator*(const ModelImage & other) const {
    return this->dot_product(other);
}

real ModelImage::operator%(const ModelImage & other) const {
    return this->squared_difference(other);
}

/** Find the cosine of the angle between two images **/
real ModelImage::operator^(const ModelImage & other) const {
    return (*this) * other / (((*this) * (*this)) * (other * other));
}

/** Add another ModelImage to this ModelImage
 *  @param other
 *      other ModelImage, must have the same dimensions
 *  @return reference to this ModelImage
 */
ModelImage & ModelImage::operator+=(const ModelImage & other) {
    if (this->size() == other.size()) {
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                (*this)[col, row] += other[col, row];
            }
        }
        return *this;
    } else {
        throw std::invalid_argument("ModelImage sizes do not match");
    }
}

ModelImage & ModelImage::operator*=(real value) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] *= value;
        }
    }
    return *this;
}

/** Divide every pixel by `value` **/
ModelImage & ModelImage::operator/=(real value) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] /= value;
        }
    }
    return *this;
}

