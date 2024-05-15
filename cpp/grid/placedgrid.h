#ifndef GRID_H
#define GRID_H

#include <cstddef>
#include "types/types.h"
#include "utils/eigen.h"
#include "grid/abstractgrid.h"
#include "grid/pixel/pixel.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>


namespace Astar {
    /** A PlacedGrid is a grid that has a physical location somewhere in the world.
     *  Its physical position and size are determined by the canonical transform of the [0, 0] -> [1, 1] square.
     *  Changing its logical size is not allowed, but its location position and size can be altered.
     */
    template<class Derived>
    class PlacedGrid: public virtual AbstractGrid {
    private:
        AffineTransform transform_;         // Affine transform of rectangle [0, 0] [1, 1] to my position
        pair<real> pixfrac_;                // Pixel fraction in both directions
    public:
        constexpr static real NegligibleOverlap = 1e-15;

        explicit PlacedGrid(const AffineTransform & transform, pair<real> pixfrac = {1.0, 1.0});

        [[nodiscard]] inline AffineTransform transform() const { return this->transform_; }
        [[nodiscard]] inline pair<real> pixfrac() const { return this->pixfrac_; }

        [[nodiscard]] inline Vector physical_centre() const { return this->transform() * this->logical_centre(); }
        [[nodiscard]] inline Vector logical_centre() const { return this->transform().linear() / 2; }

        [[nodiscard]] inline pair<real> physical_size() const { return this->physical_size_; }

        [[nodiscard]] inline real pixel_area(int col, int row) const {
            return this->pixel_full_area(col, row) * this->pixfrac().first * this->pixfrac().second;
        }
        [[nodiscard]] inline real pixel_full_area(int col, int row) const {
            (void) col; // Currently all pixels are the same
            (void) row;
            return this->transform_.linear().determinant();
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
