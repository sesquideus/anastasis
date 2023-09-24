#ifndef GRID_H
#define GRID_H

#include "../types/types.h"
#include <Eigen/Sparse>

class Grid {
private:
    unsigned int size_w_;
    unsigned int size_h_;
    real phys_w_;
    real phys_h_;
    real rotation_;
    real fill_factor_;
public:
    Grid(unsigned int width, unsigned int height, real physical_width, real physical_height, real rotation);
    Grid(const Grid & parent);

    Eigen::SparseMatrix<real> onto(const Grid & other);

    static Point<2> segment_intersection(Point<2> p0, Point<2> p1, Point<2> q0, Point<2> q1);
    static Point<2> segment_intersection_nondeg(Point<2> p, Point<2> q, Point<2> b);
};

#endif // GRID_H
