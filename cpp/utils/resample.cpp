#include "utils/resample.h"

namespace Astar {
    PlacedImage one_to_one(const PlacedImage & image) {
        auto copy = image;
        copy.zero();
        copy.naive_drizzle(image);
        return copy;
    }

    std::vector<PlacedImage> downsample_grid(
        const PlacedImage & image,
        pair<int> model_size,
        pair<int> subsamples,
        pair<real> pixfrac
    ) {
        std::vector<PlacedImage> downsampled;
        Point model_centre = {static_cast<real>(model_size.first) / 2, static_cast<real>(model_size.second) / 2};

        for (int j = 0; j < subsamples.second; ++j) {
            for (int i = 0; i < subsamples.first; ++i) {
                PlacedImage temp(model_size, AffineTransform());
                real shift_x = static_cast<real>(i) / static_cast<real>(subsamples.first);
                real shift_y = static_cast<real>(j) / static_cast<real>(subsamples.second);
                Point shift = {shift_x, shift_y};
                real downsampled_x = static_cast<real>(image.width()) / static_cast<real>(model_size.first) *
                                     (model_centre.x - shift_x);
                real downsampled_y = static_cast<real>(image.height()) / static_cast<real>(model_size.second) *
                                     (model_centre.y - shift_y);
                Point downsampled_centre = {downsampled_x, downsampled_y};

                temp.naive_drizzle(image + shift);
                fmt::print("Scaled image down to {:d} × {:d} with shift {:6.3f} {:6.3f}, pixfrac ({:6.3f}, {:6.3f})\n",
                           model_size.first, model_size.second,
                           shift_x, shift_y,
                           image.pixfrac().first, image.pixfrac().second);
                // temporarily disabled downsampled.emplace_back(downsampled_centre, image.size(), 0, pixfrac, temp.data());
            }
        }
        fmt::print("---- downsampling complete ----\n");
        return downsampled;
    }

    std::vector<PlacedImage> downsample_with_rotations(
        const PlacedImage & original,
        pair<int> model_size,
        const std::vector<std::tuple<real, real, real>> & positions,
        pair<real> pixfrac
    ) {
        std::vector<PlacedImage> downsampled;
        Point model_centre = {static_cast<real>(model_size.first) / 2, static_cast<real>(model_size.second) / 2};

        for (auto && position: positions) {
            PlacedImage temp(model_size, AffineTransform());
            Point downsampled_centre = {0, 0};

            /* temporarily disabled
            downsampled.emplace_back(downsampled_centre,
                                     pair<int>(std::get<0>(position), std::get<1>(position)),
                                     std::get<2>(position), pixfrac, temp.data());*/
        }

        return downsampled;
    }
}
