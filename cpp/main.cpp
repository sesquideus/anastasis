#include <iostream>

#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"
#include "grid/grid.h"


constexpr real TAU = 3.14159265358979232846264 * 2.0;

int main() {
    VPImplicitTree<Point, Euclidean> vpi(std::vector<Point>());

    Grid grid({20, 20}, {10, 10}, {5, 5}, 0, {1, 1});
    CanonicalGrid image(5, 5);
    grid.print();
    grid.onto_canonical(image);

    Pixel george = Pixel(Point(-0.5, -0.5), Point(-0.5, 0.5), Point(0.5, -0.5), Point(0.5, 0.5));
    Pixel harris = Pixel(
            Point(-0.5, -0.5).rotated(TAU / 8),
            Point(-0.5,  0.5).rotated(TAU / 8),
            Point( 0.5, -0.5).rotated(TAU / 8),
            Point( 0.5,  0.5).rotated(TAU / 8)
    );

    fmt::print("Start\n");
    for (unsigned int i = 0; i < 1000000; ++i)
        george.overlap(harris);

    fmt::print("Overlap {:.9f}", george.overlap(harris));

    return 0;
}
