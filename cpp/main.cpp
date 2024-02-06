#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"
#include "grid/pixel/pixel.h"
#include "grid/grid.h"
#include "grid/modelimage.h"
#include "utils/functions.h"

constexpr real DETECTOR_PIXEL_WIDTH = 0.8;
constexpr real DETECTOR_PIXEL_HEIGHT = 0.8;
//constexpr int DETECTOR_ROWS = 100;
//constexpr int DETECTOR_COLS = 28;
//constexpr int MODEL_WIDTH = 68;
//constexpr int MODEL_HEIGHT = 68;
constexpr int DETECTOR_ROWS = 256;
constexpr int DETECTOR_COLS = 256;
constexpr int MODEL_WIDTH = 256;
constexpr int MODEL_HEIGHT = 256;
constexpr real DETECTOR_PHYSICAL_WIDTH = DETECTOR_PIXEL_WIDTH * DETECTOR_COLS;
constexpr real DETECTOR_PHYSICAL_HEIGHT = DETECTOR_PIXEL_HEIGHT * DETECTOR_ROWS;
constexpr Point shift = {0, 0};


std::vector<DetectorImage> prepare_matrices() {
    std::vector<DetectorImage> images;
    for (int i = -1; i <= 1; ++i) {
        auto image = DetectorImage(
            Point(static_cast<real>(i) / 3.0, 0),
            {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
            0, {1, 1}, pair<int>(DETECTOR_COLS, DETECTOR_ROWS)
        );
        image.fill(1);
        //image.randomize();
        images.push_back(image);
    }
    for (int i = -1; i <= 1; ++i) {
        auto image = DetectorImage(
            Point(static_cast<real>(i) / 3.0, 0),
            {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
            Tau * 0.25, {1, 1}, pair<int>(DETECTOR_COLS, DETECTOR_ROWS)
        );
        //image.randomize();
        image.fill(1);
        images.push_back(image);
    }
    return images;
}

std::vector<DetectorImage> prepare_rotated() {
    std::vector<DetectorImage> images;
    constexpr int COUNT = 13;
    for (int angle = 0; angle < COUNT; ++angle) {
        auto image = DetectorImage(
            {0, 0}, {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
            static_cast<float>(angle) / COUNT * Tau, {1, 1}, pair<int>(DETECTOR_COLS, DETECTOR_ROWS)
        );
        image.fill(1.0 / COUNT);
        images.push_back(image);
    }
    return images;
}

std::vector<DetectorImage> prepare_penguins() {
    std::vector<DetectorImage> images;
    for (int dy = -1; dy <= -1; ++dy) {
        for (int dx = 0; dx <= 0; ++dx) {
            auto image = DetectorImage(
                {256, 256},
                //Point(static_cast<real>(dx) * 1.0, static_cast<real>(dy) * 1.0),
                {96, 128},
                dx * 0.2, {1, 1}, "pingvys.bmp");
            image.multiply(1);
            images.push_back(image);
        }
    }
    return images;
}

std::vector<DetectorImage> prepare_metis() {
    std::vector<DetectorImage> images;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            auto image = DetectorImage(
                    {256, 256},
                    {256 + static_cast<float>(dx) / 3 * 16, 128 + static_cast<float>(dy) / 3 * 16},
                    0, {1, 1}, "metis.bmp");
            image.multiply(1);
            images.push_back(image);
        }
    }
    return images;
}

real test_overlap() {
    Pixel george = Pixel(Point(-0.5, -0.5), Point(-0.5, 0.5), Point(0.5, -0.5), Point(0.5, 0.5));
    Pixel harris = Pixel(
        Point(-0.5, -0.5).rotated(Tau / 8),
        Point(-0.5,  0.5).rotated(Tau / 8),
        Point( 0.5, -0.5).rotated(Tau / 8),
        Point( 0.5,  0.5).rotated(Tau / 8)
    );

    return george & harris;
}

int main() {
    // std::vector<DetectorImage> six = prepare_matrices();
    //std::vector<DetectorImage> penguins = prepare_penguins();
    // time_function(test_overlap);
    /*
    for (auto && x: six) {
        //x /= DETECTOR_PIXEL_HEIGHT;
        x += Point({MODEL_WIDTH / 2, MODEL_HEIGHT / 2});
        x.print_world();
    }

    ModelImage model_image(MODEL_WIDTH, MODEL_HEIGHT);
    time_function([&]() -> real {
        model_image.drizzle(six);
        model_image.save("out.raw");
        return 0;
    });
    */
    time_function([&]() -> real {
    //    ModelImage metis(512, 512);
    //    metis.drizzle(prepare_metis());
    //    metis.save("metis.raw");
        return Tau;
    });

   // std::cout << a << b << vstack({a, b}) << std::endl;
    return 0;
}
