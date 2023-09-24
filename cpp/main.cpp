#include <iostream>

#include "types/types.h"
#include "types/point.h"
#include "spatial/metrics.h"
#include "spatial/structures/vpitree.h"

int main() {
    VPImplicitTree<Point, Euclidean> vpi(std::vector<Point>());
    return 0;
}
