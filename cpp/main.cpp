#include <chrono>

#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"
#include "grid/pixel/pixel.h"
#include "grid/grid.h"
#include "grid/modelimage.h"
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

// Simply tau = 2 * pi
constexpr real Tau = 3.14159265358979232846264 * 2.0;

constexpr real DETECTOR_PIXEL_WIDTH = 1;
constexpr real DETECTOR_PIXEL_HEIGHT = 1;
//constexpr int DETECTOR_ROWS = 100;
//constexpr int DETECTOR_COLS = 28;
//constexpr int MODEL_WIDTH = 68;
//constexpr int MODEL_HEIGHT = 68;
constexpr int DETECTOR_ROWS = 16;
constexpr int DETECTOR_COLS = 16;
constexpr int MODEL_WIDTH = 16;
constexpr int MODEL_HEIGHT = 16;
constexpr real DETECTOR_PHYSICAL_WIDTH = DETECTOR_PIXEL_WIDTH * DETECTOR_COLS;
constexpr real DETECTOR_PHYSICAL_HEIGHT = DETECTOR_PIXEL_HEIGHT * DETECTOR_ROWS;

std::vector<DetectorImage> prepare_matrices() {
    std::vector<DetectorImage> images;
    for (int i = -1; i <= 1; ++i) {
        auto image = DetectorImage(
            Point(static_cast<real>(i) / 3.0, 0),
            {DETECTOR_COLS, DETECTOR_ROWS},
            {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
            0, {1, 1}
        );
        image.fill(1);
        //image.randomize();
        images.push_back(image);
    }
    for (int i = -1; i <= 1; ++i) {
        auto image = DetectorImage(
                Point(static_cast<real>(i) / 3.0, 0),
                {DETECTOR_COLS, DETECTOR_ROWS},
                {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
                Tau * 0.25, {1, 1}
        );
        //image.randomize();
        image.fill(1);
        images.push_back(image);
    }
    return images;
}

std::vector<DetectorImage> prepare_rotated() {
    std::vector<DetectorImage> images;
    for (int angle = 0; angle < 30; ++angle) {
        auto image = DetectorImage(
                {0, 0}, {DETECTOR_COLS, DETECTOR_ROWS}, {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
                static_cast<float>(angle) / 30.0 * Tau, {1, 1}
        );
        image.fill(1.0 / 30.0);
        images.push_back(image);
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

void time_function(std::function<real(void)> f) {
    auto start = std::chrono::high_resolution_clock::now();
    fmt::print("Result is {}\n", f());
    auto diff = std::chrono::high_resolution_clock::now() - start;
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(diff);

    fmt::print("Test took {} \u03BCs\n", us.count());
}

int main() {
    std::vector<DetectorImage> six = prepare_rotated();
    time_function(test_overlap);

    for (auto && x: six) {
        x /= DETECTOR_PIXEL_HEIGHT;
        x += Point({MODEL_WIDTH / 2, MODEL_HEIGHT / 2});
        x.print_world();
    }

    ModelImage model_image(MODEL_WIDTH, MODEL_HEIGHT);
    time_function([&]() -> real { model_image.drizzle(six); return 0; });

   // std::cout << a << b << vstack({a, b}) << std::endl;



    return 0;
}
