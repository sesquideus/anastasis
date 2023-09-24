#include "grid.h"

Grid::Grid(unsigned int width, unsigned int height, real physical_width, real physical_height, real rotation):
    _size_w(width),
    _size_h(height),
    _phys_w(physical_width),
    _phys_h(physical_height),
    _rotation(rotation)
{ }

Eigen::SparseMatrix<real> Grid::onto(const Grid & other) {
    /* Find the overlap for every pair of pixels */
    return Eigen::SparseMatrix<real>();
}

Point2D Grid::segment_intersection_nondegenerate(Point2D p0, Point2D p1, Point2D q0, Point2D q1) {
    return nullptr;
}
