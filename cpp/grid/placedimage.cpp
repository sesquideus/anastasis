#include "placedimage.h"

namespace Astar {
    PlacedImage::PlacedImage(const AffineTransform & transform, pair<int> size, pair<real> pixfrac):
        AbstractGrid(size),
        PlacedGrid(transform, pixfrac),
        Image(size)
    {}

    PlacedImage::PlacedImage(const AffineTransform & transform, const Matrix & data, pair<real> pixfrac):
        AbstractGrid(data.cols(), data.rows()),
        PlacedGrid(transform, pixfrac),
        Image(data)
    {}

    PlacedImage::PlacedImage(const AffineTransform & transform, const std::string & filename, pair<real> pixfrac):
        AbstractGrid(read_bitmap_header(filename)),
        PlacedGrid(transform, pixfrac),
        Image(filename)
    {}

    Pixel PlacedImage::pixel(int x, int y) const {
        if ((x < 0) || (x >= this->width()) || (y < 0) || (y >= this->height())) {
            return Pixel::invalid();
        } else {
            real left = static_cast<real>(x);
            real right = left + 1.0;
            real bottom = static_cast<real>(y);
            real top = bottom + 1.0;
            return Pixel(this->transform() * Pixel(
                {left, bottom},
                {right, bottom},
                {left, top},
                {right, top}
            ).corners());
        }
    }


    PlacedImage & PlacedImage::naive_drizzle(const Astar::PlacedImage & image) {
        real total = 0;
        real sum = 0;
        int inspected = 0;

        for (int row = 0; row < image.height(); ++row) {
            for (int col = 0; col < image.width(); ++col) {
                // Find the orthogonal bounds of the pixel so that many unnecessary computations can be avoided completely
                const Pixel & image_pixel = image.world_pixel(col, row);
                Box bounds = image_pixel.bounding_box();

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
                        if (overlap > PlacedImage::NegligibleOverlap) {
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

    PlacedImage & PlacedImage::naive_drizzle(const std::vector<PlacedImage> & images) {
        for (auto && image: images) {
            this->naive_drizzle(image);
        }
        return *this;
    }
} // Astar