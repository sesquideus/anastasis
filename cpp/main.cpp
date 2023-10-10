#include <chrono>

#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"
#include "grid/grid.h"
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>

constexpr real TAU = 3.14159265358979232846264 * 2.0;

constexpr real DETECTOR_PIXEL_WIDTH = 20.7;
constexpr real DETECTOR_PIXEL_HEIGHT = 8.2;
//constexpr int DETECTOR_ROWS = 100;
//constexpr int DETECTOR_COLS = 28;
//constexpr int MODEL_WIDTH = 68;
//constexpr int MODEL_HEIGHT = 68;
constexpr int DETECTOR_ROWS = 10;
constexpr int DETECTOR_COLS = 3;
constexpr int MODEL_WIDTH = 5;
constexpr int MODEL_HEIGHT = 5;
constexpr real DETECTOR_PHYSICAL_WIDTH = DETECTOR_PIXEL_WIDTH * DETECTOR_COLS;
constexpr real DETECTOR_PHYSICAL_HEIGHT = DETECTOR_PIXEL_HEIGHT * DETECTOR_ROWS;

std::vector<Grid> prepare_matrices() {
    std::vector<Grid> grids;
    for (int i = -1; i <= 1; ++i) {
        grids.push_back({Point(static_cast<real>(i) / 3.0, 0),
                        {DETECTOR_COLS, DETECTOR_ROWS}, {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
                        0, {1, 1}});
    }
    for (int i = -1; i <= 1; ++i) {
        grids.push_back({Point(static_cast<real>(i) / 3.0, 0),
                         {DETECTOR_COLS, DETECTOR_ROWS}, {DETECTOR_PHYSICAL_WIDTH, DETECTOR_PHYSICAL_HEIGHT},
                         0.25 * TAU, {1, 1}});
    }
    return grids;
}


int main() {
    std::vector<Grid> grids = prepare_matrices();

    CanonicalGrid image(MODEL_WIDTH, MODEL_HEIGHT);

    Pixel george = Pixel(Point(-0.5, -0.5), Point(-0.5, 0.5), Point(0.5, -0.5), Point(0.5, 0.5));
    Pixel harris = Pixel(
            Point(-0.5, -0.5).rotated(TAU / 8),
            Point(-0.5,  0.5).rotated(TAU / 8),
            Point( 0.5, -0.5).rotated(TAU / 8),
            Point( 0.5,  0.5).rotated(TAU / 8)
    );
    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned int i = 0; i < 1; ++i)
        (void) george.overlap(harris);

    //fmt::print("Overlap {:.12f}\n", george.overlap(harris));

    fmt::print("Start\n");
    std::vector<Eigen::SparseMatrix<real>> matrices;
    for (auto && grid: grids) {
        grid.print();
        // Shift to the centre of the canonical grid
        grid += Point(static_cast<real>(MODEL_WIDTH) / 2, static_cast<real>(MODEL_HEIGHT) / 2);
        grid /= DETECTOR_PIXEL_HEIGHT;                // Scale down by pixel size in arcsec
        grid.print_world();
        matrices.push_back(grid.onto_canonical(image));
        std::cout << matrices.back() << std::endl;
    }
    auto A = hstack(matrices);
    auto diff = std::chrono::high_resolution_clock::now() - start;
    auto t1 = std::chrono::duration_cast<std::chrono::microseconds>(diff);
    fmt::print("Matrix with size {}×{} (total {} elements) has {} nonzero elements, computed in {} \u03BCs\n",
                A.rows(), A.cols(), A.size(), A.nonZeros(), t1.count());

    start = std::chrono::high_resolution_clock::now();
    fmt::print("{}\n", A.sum() / 6);

    Eigen::SparseMatrix<real> C(MODEL_WIDTH * MODEL_HEIGHT, MODEL_WIDTH * MODEL_HEIGHT);
    C.setIdentity();
    C.makeCompressed();
    fmt::print("C is now an identity matrix with size {}\n", C.nonZeros());

    auto L = A.transpose() * C * A;
    fmt::print("{}×{} elements in L\n", L.rows(), L.cols());
    std::cout << A << std::endl;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<real>, Eigen::Lower|Eigen::Upper> solver;
    solver.compute(L);
    fmt::print("L is now computed\n");

    Eigen::SparseMatrix<real> I(grids.size() * DETECTOR_COLS * DETECTOR_ROWS, grids.size() * DETECTOR_COLS * DETECTOR_ROWS);
    I.setIdentity();
    I.makeCompressed();
    auto Linv = solver.solve(I);

    std::cout << L * Linv << std::endl;

    auto t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
    fmt::print("Multiplication took {} \u03BCs\n", t2.count());

    Eigen::SparseMatrix<real> a(5, 5), b(5, 5);
    a.insert(4, 3) = 1;
    a.insert(1, 0) = 3;
    a.insert(2, 4) = 7;
    b.insert(1, 3) = 2;
    b.insert(2, 3) = 4;
    b.insert(4, 0) = 9;
   // std::cout << a << b << vstack({a, b}) << std::endl;



    return 0;
}
