#ifndef GRID_H
#define GRID_H

#include <cstddef>
#include "../types/types.h"
#include "grid/pixel/pixel.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include "canonicalgrid.h"
#include <Eigen/Sparse>


typedef std::tuple<long, long, long, long, real> Overlap4D;
typedef Eigen::Matrix<real, Eigen::Dynamic, Eigen::Dynamic> Matrix;


class Grid {
private:
    Point centre_;
    std::size_t size_w_;
    std::size_t size_h_;
    real phys_w_;
    real phys_h_;
    real rotation_;
    real pixfrac_x_;
    real pixfrac_y_;
    real pixel_width_;
    real pixel_height_;
    constexpr static real NegligibleOverlap = 1e-15;
public:
    Grid(Point centre, std::pair<std::size_t, std::size_t> grid_size, std::pair<real, real> physical_size,
         real rotation, std::pair<real, real> pixfrac);
    static Grid from_pixel_size(Point centre,
                                std::pair<std::size_t, std::size_t> grid_size,
                                std::pair<real, real> pixel_size,
                                real rotation,
                                std::pair<real, real> pixfrac);

    void load(const std::vector<std::vector<real>> & data);

    [[nodiscard]] inline int width() const { return this->size_w_; }
    [[nodiscard]] inline int height() const { return this->size_h_; }

    [[nodiscard]] Point grid_centre(unsigned int x, unsigned int y) const;
    [[nodiscard]] Point world_centre(unsigned int x, unsigned int y) const;

    [[nodiscard]] Pixel grid_pixel(unsigned int x, unsigned int y) const;
    [[nodiscard]] Pixel world_pixel(unsigned int x, unsigned int y) const;

    [[nodiscard]] std::vector<Overlap4D> onto_canonical(const CanonicalGrid & canonical) const;
    [[nodiscard]] Eigen::SparseMatrix<real> matrix_canonical(const CanonicalGrid & canonical) const;

    void print() const;
    void print_world() const;

    Grid & operator+=(Point shift);
    Grid & operator-=(Point shift);
    Grid & operator*=(real scale);
    Grid & operator/=(real scale);
};

Eigen::SparseMatrix<real> stack(const std::vector<Eigen::SparseMatrix<real>> & matrices, bool vertical);
Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> hstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> vstack2(std::vector<Eigen::SparseMatrix<real>> matrices);

#endif // GRID_H
