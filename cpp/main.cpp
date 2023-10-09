#include <chrono>

#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"
#include "grid/grid.h"


constexpr real TAU = 3.14159265358979232846264 * 2.0;

int main() {
    std::vector<Grid> grids;
    grids.reserve(6);

    for (int i = -1; i <= 1; ++i) {
        grids.push_back({Point(static_cast<real>(i) / 3.0, 0), {28, 100}, {579.6, 820}, 0, {1, 1}});
    }
    for (int i = -1; i <= 1; ++i) {
        grids.push_back({Point(static_cast<real>(i) / 3.0, 0), {28, 100}, {579.6, 820}, 0.25 * TAU, {1, 1}});
    }

    CanonicalGrid image(68, 68);

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
        grid += Point(34, 34);
        grid /= 8.2;
        grid.print_world();
        matrices.push_back(grid.onto_canonical(image));
    }
    auto matrix = vstack(matrices);
    auto diff = std::chrono::high_resolution_clock::now() - start;
    auto t1 = std::chrono::duration_cast<std::chrono::microseconds>(diff);
    fmt::print("Matrix with size {}×{} (total {} elements) has {} nonzero elements, computed in {} \u03BCs\n",
                matrix.rows(), matrix.cols(), matrix.size(), matrix.nonZeros(), t1.count());

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
