#include <iostream>

#include "grid/detectorimage.h"
#include "grid/modelimage.h"


ModelImage one_to_one(const DetectorImage & image) {
    auto copy = image;
    ModelImage clone(image.size().first, image.size().second);
    copy.set_centre({static_cast<real>(image.size().first) / 2.0, static_cast<real>(image.size().second) / 2.0});
    copy.set_physical_size(image.size());
    fmt::print("Detector: {}\n", copy);
    fmt::print("Model: {}\n", clone);
    clone.naive_drizzle(copy);
    clone.save_npy("out/one-to-one.npy");
    fmt::print("---- one to one complete ----\n");
    return clone;
}

/** Take a detector image and create <subsamples_x> * <subsamples_y> downsampled images with resolution
 *  <model_width> * <model_height>, each shifted by a corresponding fraction of a *new* pixel size in both directions.
 *  This can be then used to reconstruct the original image.
 */
std::vector<std::vector<DetectorImage>> downsample(
    const DetectorImage & image,
    int model_width, int model_height,
    int subsamples_x, int subsamples_y,
    real pixfrac_x, real pixfrac_y
) {
    std::vector<std::vector<DetectorImage>> downsampled;
    Point model_centre = {static_cast<real>(model_width) / 2, static_cast<real>(model_height) / 2};

    for (int j = 0; j < subsamples_y; ++j) {
        downsampled.emplace_back();
        for (int i = 0; i < subsamples_x; ++i) {
            ModelImage temp(model_width, model_height);
            real shift_x = static_cast<real>(i) / static_cast<real>(subsamples_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(subsamples_y);
            Point shift = {shift_x, shift_y};
            real downsampled_x = static_cast<real>(image.width()) / static_cast<real>(model_width) * (model_centre.x - shift_x);
            real downsampled_y = static_cast<real>(image.height()) / static_cast<real>(model_height) * (model_centre.y - shift_y);
            Point downsampled_centre = {downsampled_x, downsampled_y};

            temp.naive_drizzle(image + shift);
            fmt::print("Scaled down to {:d} × {:d} with shift {:6.3f} {:6.3f}, pixfrac ({:6.3f}, {:6.3f})\n",
                       model_width, model_height, shift_x, shift_y, image.pixfrac().first, image.pixfrac().second);
            downsampled[j].emplace_back(downsampled_centre,
                                        pair<int>(image.width(), image.height()),
                                        0, pair<real>(pixfrac_x, pixfrac_y), temp.data());
        }
    }
    fmt::print("---- downsampling complete ----\n");
    return downsampled;
}

SparseMatrix overlap_matrix(
    const std::vector<std::vector<DetectorImage>> & downsampled,
    pair<int> output_size)
{
    ModelImage output(output_size);
    auto images = flatten(downsampled);
    std::vector<SparseMatrix> matrices;
    fmt::print("Downsampled {:d}\n", images.size());

    for (auto const & image: images) {
        matrices.emplace_back(output.overlap_matrix(image));
    }

    SparseMatrix total = hstack(matrices);
    return total;
}

ModelImage drizzle(
    const std::vector<std::vector<DetectorImage>> & downsampled,    // 2D vector of images to drizzle
    pair<int> output_size                                           // output size of the grid, [0, x), [0, y)
) {
    ModelImage drizzled(output_size.first, output_size.second);
    unsigned int samples_x = downsampled.size();
    unsigned int samples_y = downsampled[0].size();

    for (unsigned int j = 0; j < samples_y; ++j) {
        for (unsigned int i = 0; i < samples_x; ++i) {
            real shift_x = static_cast<real>(i) / static_cast<real>(samples_x);
            real shift_y = static_cast<real>(j) / static_cast<real>(samples_y);
            real scale_x = static_cast<real>(output_size.first) / downsampled[i][j].width();
            real scale_y = static_cast<real>(output_size.second) / downsampled[i][j].height();
            drizzled.naive_drizzle(downsampled[i][j]);
            fmt::print("Drizzled with shifts {:6.3f} {:6.3f} (pixels {:6.3f} {:6.3f}) "
                       "at {:6.3f} {:6.3f}), pixfrac {:6.3f} {:6.3f}\n",
                       shift_x, shift_y,
                       shift_x * scale_x, shift_y * scale_y,
                       downsampled[i][j].centre().x, downsampled[i][j].centre().y,
                       downsampled[i][j].pixfrac().first, downsampled[i][j].pixfrac().second);
        }
    }
    return drizzled;
}

void print_usage(int code) {
    fmt::print("Usage: subpixel <filename> model_size_x model_size_y subpixel_shifts_x subpixel_shifts_y "
               "pixfrac_x pixfrac_y\n");
    fmt::print("<filename>          path to an 8-bit bmp file\n");
    fmt::print("model_size_x        int > 0, number of pixels in horizontal direction for downsampled images\n");
    fmt::print("model_size_y        int > 0, number of pixels in vertical direction for downsampled images\n");
    fmt::print("subpixel_shifts_x   int > 0, number of downsampled images to produce in horizontal direction\n");
    fmt::print("subpixel_shifts_y   int > 0, number of downsampled images to produce in vertical direction\n");
    fmt::print("pixfrac_x           float (0, 1], pixel fraction used in drizzling, horizontal direction "
               "(recommended 1 / subpixel_shifts_x)\n");
    fmt::print("pixfrac_y           float (0, 1], pixel fraction used in drizzling, vertical direction "
               "(recommended 1 / subpixel_shifts_y)\n");
    std::exit(code);
}

int main(int argc, char * argv[]) {
    std::string exec_name = argv[0];
    std::vector<std::string> args(argv + 1, argv + argc);

    int model_width;
    int model_height;
    int subshifts_x;
    int subshifts_y;
    real pixfrac_x;
    real pixfrac_y;

    try {
        if (argc != 8) {
            print_usage(0);
        }
        model_width = std::stoi(args[1]);
        model_height = std::stoi(args[2]);
        subshifts_x = std::stoi(args[3]);
        subshifts_y = std::stoi(args[4]);
        pixfrac_x = std::stof(args[5]);
        pixfrac_y = std::stof(args[6]);
    } catch (std::invalid_argument & exc) {
        fmt::print("Aborting due to invalid argument type: {}\n", exc.what());
        print_usage(1);
    }

    Point centre = {static_cast<real>(model_width) / 2, static_cast<real>(model_height) / 2};

    try {
        DetectorImage input(centre, {model_width, model_height}, 0, {1, 1}, args[0]);
        input.set_centre({input.physical_size().first / 2, input.physical_size().second / 2});

        auto size = input.size();
        real scale_x = static_cast<real>(size.first) / model_width;
        real scale_y = static_cast<real>(size.second) / model_height;
        fmt::print("About to drizzle {} (image size {} × {}) down to model with size {} × {}, placed at {}\n",
                   args[0], size.first, size.second, model_width, model_height, centre);
        fmt::print("Pixel scaling factors are x = {}, y = {}\n", scale_x, scale_y);
        fmt::print("Pixfrac is px = {}, py = {}\n", pixfrac_x, pixfrac_y);

        auto clone = one_to_one(input);
        input.set_centre(centre);
        input.set_physical_size({model_width, model_height});

        auto downsampled = downsample(input, model_width, model_height, subshifts_x, subshifts_y, pixfrac_x, pixfrac_y);
        // Save one of the downsampled images so that we can plot it and see what it looks like
        downsampled[0][0].save_npy("out/downsampled.npy");

        overlap_matrix(downsampled, {model_width, model_height});

        auto drizzled = drizzle(downsampled, input.size());
        drizzled.save_npy("out/drizzled.npy");

        real similarity = clone ^ drizzled;
        fmt::print("Similarity score is {}\n", similarity);

        real angle = (clone * drizzled) / (std::sqrt(drizzled * drizzled) * std::sqrt(clone * clone));
        fmt::print("Angle difference is {}\n", std::acos(angle));
    } catch (std::runtime_error & exc) {
        fmt::print("Could not map one to one: {}\n", exc.what());
        std::exit(3);
    }
}