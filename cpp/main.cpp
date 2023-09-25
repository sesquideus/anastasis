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
    return 0;
}
