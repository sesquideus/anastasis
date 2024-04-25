#include "utils/resample.h"

ModelImage one_to_one(const DetectorImage & image) {
    auto copy = image;
    ModelImage clone(image.size().first, image.size().second);
    copy.set_centre({static_cast<real>(image.size().first) / 2.0, static_cast<real>(image.size().second) / 2.0});
    copy.set_physical_size(image.size());
    fmt::print("\tDetector: {}\n", copy);
    fmt::print("\tModel: {}\n", clone);
    clone.naive_drizzle(copy);
    clone.save_npy("out/one-to-one.npy");
    fmt::print("---- one to one complete ----\n");
    return clone;
}

std::vector<std::vector<DetectorImage>> downsample_grid(
    const DetectorImage & image,
    pair<int> model_size,
    pair<int> subsamples,
    pair<real> pixfrac
) {
    std::vector<std::vector<DetectorImage>> downsampled;
    Point model_centre = {static_cast<real>(model_size.first) / 2, static_cast<real>(model_size.second) / 2};

    for (int j = 0; j < subsamples.second; ++j) {
        downsampled.emplace_back();
        for (int i = 0; i < subsamples.first; ++i) {
            ModelImage temp(model_size);
            real shift_x = static_cast<real>(i) / static_cast<real>(subsamples.first);
            real shift_y = static_cast<real>(j) / static_cast<real>(subsamples.second);
            Point shift = {shift_x, shift_y};
            real downsampled_x = static_cast<real>(image.width()) / static_cast<real>(model_size.first) *
                                 (model_centre.x - shift_x);
            real downsampled_y = static_cast<real>(image.height()) / static_cast<real>(model_size.second) *
                                 (model_centre.y - shift_y);
            Point downsampled_centre = {downsampled_x, downsampled_y};

            temp.naive_drizzle(image + shift);
            fmt::print("Scaled down to {:d} × {:d} with shift {:6.3f} {:6.3f}, pixfrac ({:6.3f}, {:6.3f})\n",
                       model_size.first, model_size.second,
                       shift_x, shift_y,
                       image.pixfrac().first, image.pixfrac().second);
            downsampled[j].emplace_back(downsampled_centre, image.size(), 0, pixfrac, temp.data());
        }
    }
    fmt::print("---- downsampling complete ----\n");
    return downsampled;
}

/** Take an original image and downsample_grid it to many small subexposures at different rotations and positions
 *  (currently of the same size)
 *  For instance for METIS we want three subsamples in horizontal direction and three subsamples rotated 90°.
 */
std::vector<DetectorImage> downsample_with_rotations(
    const DetectorImage & original,
    pair<int> model_size,
    const std::vector<std::tuple<real, real, real>> & positions,
    pair<real> pixfrac
) {
    std::vector<DetectorImage> downsampled;
    Point model_centre = {static_cast<real>(model_size.first) / 2, static_cast<real>(model_size.second) / 2};

    for (auto && position: positions) {
        ModelImage temp(model_size);
        Point downsampled_centre = {0, 0};

        downsampled.emplace_back(downsampled_centre,
                                 pair<int>(std::get<0>(position), std::get<1>(position)),
                                 std::get<2>(position), pixfrac, temp.data());
    }

    return downsampled;
}
