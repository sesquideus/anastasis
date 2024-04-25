#ifndef GRID_H
#define GRID_H

#include <cstddef>
#include "../types/types.h"
#include "grid/abstractgrid.h"
#include "grid/pixel/pixel.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>


/** A PlacedGrid is a grid that has a physical location somewhere in the world:
 *  it has a central point, physical extent in both directions and a rotation.
 *  Changing its logical size is still not possible, but its location can be altered.
 */
class PlacedGrid: public virtual AbstractGrid {
private:
    Point centre_;
    real phys_w_;
    real phys_h_;
    real rotation_;
    real pixfrac_x_;
    real pixfrac_y_;
    real pixel_width_;
    real pixel_height_;
public:
    constexpr static real NegligibleOverlap = 1e-15;

    PlacedGrid(Point centre,
               pair<int> grid_size,
               pair<real> physical_size,
               real rotation,
               pair<real> pixfrac);
    static PlacedGrid from_pixel_size(Point centre,
                                      pair<int> grid_size,
                                      pair<real> pixel_size,
                                      real rotation,
                                      pair<real> pixfrac);

    [[nodiscard]] inline Point centre() const { return this->centre_; }
    [[nodiscard]] inline pair<real> physical_size() const { return {this->phys_w_, this->phys_h_}; }
    [[nodiscard]] inline real rotation() const { return this->rotation_; }
    [[nodiscard]] inline pair<real> pixfrac() const { return {this->pixfrac_x_, this->pixfrac_y_}; }

    [[nodiscard]] inline real pixel_area(int col, int row) const {
        (void) col;
        (void) row;
        return this->pixel_width_ * this->pixel_height_;
    }

    PlacedGrid & set_centre(Point centre);
    PlacedGrid & set_physical_size(pair<real> size);

    [[nodiscard]] Point grid_centre(unsigned int x, unsigned int y) const;
    [[nodiscard]] Point world_centre(unsigned int x, unsigned int y) const;

    [[nodiscard]] Pixel grid_pixel(unsigned int x, unsigned int y) const;
    [[nodiscard]] Pixel world_pixel(unsigned int x, unsigned int y) const;

    //[[nodiscard]] std::vector<Overlap4D> onto_canonical(const ModelImage & canonical) const;
    //[[nodiscard]] Eigen::SparseMatrix<real> matrix_canonical(const ModelImage & canonical) const;

    void print_world() const;

    PlacedGrid & operator+=(Point shift);
    PlacedGrid & operator-=(Point shift);
    PlacedGrid & scale(real scale);
    PlacedGrid & scale(pair<real> scale);
    PlacedGrid & operator<<=(real angle);
    PlacedGrid & operator>>=(real angle);
};

PlacedGrid operator+(const PlacedGrid & grid, Point shift);

Eigen::SparseMatrix<real> stack(const std::vector<Eigen::SparseMatrix<real>> & matrices, bool vertical);
Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> hstack(const std::vector<Eigen::SparseMatrix<real>> & matrices);
Eigen::SparseMatrix<real> vstack2(std::vector<Eigen::SparseMatrix<real>> matrices);

#endif // GRID_H
