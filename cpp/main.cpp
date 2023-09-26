#include <iostream>

#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"
#include "grid/grid.h"

int main() {
    VPImplicitTree<Point, Euclidean> vpi(std::vector<Point>());

    Grid grid({20, 20}, {10, 10}, {5, 5}, 0.1, {1, 1});
    CanonicalGrid image(5, 5);
    grid.print();
    grid.onto_canonical(image);

    Pixel george = Pixel(Point(0.1, 0), Point(0.1, 1), Point(1.1, 0), Point(1.1, 1));
    Pixel harris = Pixel(Point(0, 0.5), Point(0, 1.5), Point(1, 0.5), Point(1, 1.5));
    fmt::print("Overlap {:.6f}", george.overlap(harris));

    return 0;
}
