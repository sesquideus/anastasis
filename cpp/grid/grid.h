#ifndef GRID_H
#define GRID_H

#include "../types/types.h"
#include "pixel.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include "canonicalgrid.h"
#include <Eigen/Sparse>


typedef std::tuple<long, long, long, long, real> Overlap4D;


class Grid {
private:
    Point centre_;
    int size_w_;
    int size_h_;
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

    [[nodiscard]] inline int width() const { return this->size_w_; }
    [[nodiscard]] inline int height() const { return this->size_h_; }

    [[nodiscard]] Point grid_centre(unsigned int x, unsigned int y) const;
    [[nodiscard]] Point world_centre(unsigned int x, unsigned int y) const;

    [[nodiscard]] Pixel grid_pixel(unsigned int x, unsigned int y) const;
    [[nodiscard]] Pixel world_pixel(unsigned int x, unsigned int y) const;

    [[nodiscard]] Eigen::SparseMatrix<real> onto_canonical(const CanonicalGrid & canonical) const;

    void print() const;
    void print_world() const;

    Grid & operator+=(Point shift);
    Grid & operator-=(Point shift);
    Grid & operator*=(real scale);
    Grid & operator/=(real scale);
};

Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> vstack2(std::vector<Eigen::SparseMatrix<real>> matrices);

#endif // GRID_H
