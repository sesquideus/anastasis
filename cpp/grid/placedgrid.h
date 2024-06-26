#ifndef GRID_H
#define GRID_H

#include <cstddef>
#include "types/types.h"
#include "utils/eigen.h"
#include "grid/abstractgrid.h"
#include "grid/pixel/pixel.h"
#include "grid/transform/affine.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>


namespace Astar {
    /** A PlacedGrid is a grid that has a physical location somewhere in the world:
     *  it has a central point, physical extent in both directions and a rotation.
     *  Changing its logical size is still not allowed, but its location can be altered.
     */
    template<class Derived>
    class PlacedGrid: public virtual AbstractGrid {
    private:
        Point centre_;
        pair<real> physical_size_;
        pair<real> pixfrac_;
        real rotation_;
        pair<real> pixel_size_;
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
        [[nodiscard]] inline pair<real> physical_size() const { return this->physical_size_; }
        [[nodiscard]] inline real rotation() const { return this->rotation_; }
        [[nodiscard]] inline pair<real> pixfrac() const { return this->pixfrac_; }

        [[nodiscard]] inline real pixel_area(int col, int row) const {
            (void) col;
            (void) row;
            return this->pixel_size_.first * this->pixel_size_.second * this->pixfrac_.first * this->pixfrac_.second;
        }

        Derived & set_centre(Point centre);
        Derived & set_physical_size(pair<real> size);
        Derived & set_physical_size(real width, real height);

       // Derived inverse_transform(const PlacedGrid<Derived> & other) const;

        [[nodiscard]] Point grid_centre(unsigned int x, unsigned int y) const;
        [[nodiscard]] Point world_centre(unsigned int x, unsigned int y) const;

        [[nodiscard]] Pixel grid_pixel(unsigned int x, unsigned int y) const;
        [[nodiscard]] Pixel world_pixel(unsigned int x, unsigned int y) const;

        //[[nodiscard]] std::vector<Overlap4D> onto_canonical(const ModelImage & canonical) const;
        //[[nodiscard]] Eigen::SparseMatrix<real> matrix_canonical(const ModelImage & canonical) const;

        void print_world() const;
        void print_corners() const;

        // Shift the whole PlacedGrid by a constant vector
        Derived & operator+=(Point shift);
        Derived & operator-=(Point shift);
        // Scale the PlacedGrid by constant in both directions
        Derived & scale(real scale);
        Derived & scale(pair<real> scale);
        Derived & operator<<=(real angle);
        Derived & operator>>=(real angle);

        Derived & transform(const AffineTransform & transform);
    };

    /* template<class Derived>
     Derived PlacedGrid<Derived>::inverse_transform(const PlacedGrid<Derived> & other) const {
         return PlacedGrid<Derived>( this->rotation - other.rotation_);
     }*/

    template<class Derived>
    Derived operator+(Derived grid, const Point & shift) { return grid += shift; }

    template<class Derived>
    Derived operator-(Derived grid, const Point & shift) { return grid -= shift; }

    template<class Derived>
    Derived operator<<(Derived grid, real angle) { return grid <<= angle; }

    template<class Derived>
    Derived operator>>(Derived grid, real angle) { return grid >>= angle; }
}

#include "grid/placedgrid.tpp"

#endif // GRID_H
