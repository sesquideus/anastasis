#ifndef GRID_H
#define GRID_H

#include "../types/types.h"
#include "pixel.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include "canonicalgrid.h"
#include <Eigen/Sparse>

class Grid {
private:
    Point centre_;
    unsigned int size_w_;
    unsigned int size_h_;
    real phys_w_;
    real phys_h_;
    real rotation_;
    real pixfrac_x_;
    real pixfrac_y_;
    real pixel_width_;
    real pixel_height_;
public:
    Grid(Point centre, std::pair<unsigned int, unsigned int> grid_size, std::pair<real, real> physical_size, real rotation, std::pair<real, real> pixfrac);
//    Grid(const Grid && parent);

    [[nodiscard]] inline unsigned int width() const { return this->size_w_; }
    [[nodiscard]] inline unsigned int height() const { return this->size_h_; }

    Point grid_centre(unsigned int x, unsigned int y) const;
    Point world_centre(unsigned int x, unsigned int y) const;

    Pixel grid_pixel(unsigned int x, unsigned int y) const;
    Pixel world_pixel(unsigned int x, unsigned int y) const;

    Eigen::SparseMatrix<real> onto_canonical(const CanonicalGrid & canonical) const;

    void print() const;
    void print_world() const;

    Grid & operator+=(Point shift);
    Grid & operator-=(Point shift);
    Grid & operator*=(real scale);
    Grid & operator/=(real scale);
};


#endif // GRID_H
