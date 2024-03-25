#include "abstractgrid.h"

AbstractGrid::AbstractGrid(int width, int height):
    size_w_(width),
    size_h_(height)
{}

AbstractGrid::AbstractGrid(pair<int> size):
    AbstractGrid(size.first, size.second)
{}
