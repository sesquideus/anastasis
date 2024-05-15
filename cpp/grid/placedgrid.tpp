#include "placedgrid.h"
#include <iostream>

namespace Astar {
    explicit PlacedGrid(const AffineTransform & transform, pair<real> pixfrac = {1.0, 1.0}):
        AbstractGrid(transform.linear()),
        pixfrac_(pixfrac)
    {}

    template<class Derived>
    Point PlacedGrid<Derived>::grid_centre(unsigned int x, unsigned int y) const {
        return {
            ((static_cast<real>(x) + 0.5) / static_cast<real>(this->width()) - 0.5) * this->physical_size_.first,
            ((static_cast<real>(y) + 0.5) / static_cast<real>(this->height()) - 0.5) * this->physical_size_.second
        };
    }

    template<class Derived>
    Point PlacedGrid<Derived>::world_centre(unsigned int x, unsigned int y) const {
        Point grid_centre = this->grid_centre(x, y);
        real sina = std::sin(this->rotation_);
        real cosa = std::cos(this->rotation_);
        return {grid_centre.x * cosa - grid_centre.y * sina, grid_centre.x * sina + grid_centre.y * cosa};
    }

    template<class Derived>
    Pixel PlacedGrid<Derived>::grid_pixel(unsigned int x, unsigned int y) const {
        const Point & grid_centre = this->grid_centre(x, y);
        real half_width = this->pixel_size_.first * this->pixfrac_.first * 0.5;
        real half_height = this->pixel_size_.second * this->pixfrac_.second * 0.5;
        return Pixel(
            {grid_centre.x - half_width, grid_centre.y - half_height},
            {grid_centre.x + half_width, grid_centre.y - half_height},
            {grid_centre.x - half_width, grid_centre.y + half_height},
            {grid_centre.x + half_width, grid_centre.y + half_height}
        );
    }

    template<class Derived>
    Pixel PlacedGrid<Derived>::world_pixel(unsigned int x, unsigned int y) const {
        return this->transform_ * this->world_pixel();
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::set_centre(Point centre) {
        this->centre_ = centre;
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::set_physical_size(pair<real> size) {
        this->physical_size_ = size;
        this->pixel_size_ = {
                this->pixfrac_.first * this->physical_size_.first / static_cast<real>(this->size().first),
                this->pixfrac_.second * this->physical_size_.second / static_cast<real>(this->size().second)
        };
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::set_physical_size(real width, real height) {
        return this->set_physical_size({width, height});
    }

    /**
    std::vector<Overlap4D> Grid::onto_canonical(const ModelImage & canonical) const {
        std::vector<Overlap4D> active_pixels;
        // There will be about four times as many overlaps as there are model pixels
        active_pixels.reserve(4 * canonical.size());

        for (std::size_t i = 0; i < this->size_w_; ++i) {
            for (std::size_t j = 0; j < this->size_h_; ++j) {
                auto p = this->world_pixel(i, j).bounding_box();
                //fmt::print("{} {} {} {} {} {}\n", this->width(), this->height(), p.left(), p.right(), p.bottom(), p.top());

                for (int x = std::max(0, p.left()); x < std::min(canonical.width(), p.right()); ++x) {
                    for (int y = std::max(0, p.bottom()); y < std::min(canonical.height(), p.top()); ++y) {
                        real overlap = this->world_pixel(i, j).overlap(canonical.pixel(x, y));
                        //fmt::print("{} {} -> {}\n", this->world_pixel(i, j), CanonicalGrid::pixel(x, y), overlap);
                        // Add as an active pixel if the overlap is larger than some very small value
                        if (overlap > Grid::NegligibleOverlap) {
                            active_pixels.emplace_back(x, y, i, j, overlap);
                        }
                    }
                }
            }
        }
        return active_pixels;
    } **/

    /** Compute the overlap as a 2D matrix (huge size, even if sparse) **/ /**
    Eigen::SparseMatrix<real> Grid::matrix_canonical(const ModelImage & canonical) const {
        // Allocate the output matrix
        Eigen::SparseMatrix<real> matrix(canonical.width() * canonical.height(), this->size_w_ * this->size_h_);
        matrix.reserve(4 * canonical.size());

        for (auto && pixel: this->onto_canonical(canonical)) {
            matrix.insert(
                canonical.width() * std::get<1>(pixel) + std::get<0>(pixel),
                this->width() * std::get<3>(pixel) + std::get<2>(pixel)
            ) = std::get<4>(pixel);
        }
        matrix.makeCompressed();
        return matrix;
    } */

    template<class Derived>
    void PlacedGrid<Derived>::print_world() const {
        fmt::print("Grid at {}, physical size {}, logical size {}, rotation {}, pixfrac {}\n",
                   this->centre(), this->physical_size(), this->size(), this->rotation(), this->pixfrac()
        );
    }

    template<class Derived>
    void PlacedGrid<Derived>::print_corners() const {
        Point bottomleft = this->world_pixel(0, 0).a();
        Point bottomright = this->world_pixel(this->width() - 1, 0).b();
        Point topleft = this->world_pixel(0, this->height() - 1).d();
        Point topright = this->world_pixel(this->width() - 1, this->height() - 1).c();
        fmt::print("Grid corners: {}, {}, {}, {}\n",
                   bottomleft, bottomright, topleft, topright);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::operator+=(Point shift) {
        this->centre_ += shift;
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::operator-=(Point shift) {
        this->centre_ -= shift;
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::scale(real scale) {
        return this->scale({scale, scale});
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::scale(pair<real> scale) {
        this->phys_w_ *= scale.first;
        this->phys_h_ *= scale.second;
        this->pixel_width_ *= scale.first;
        this->pixel_height_ *= scale.second;
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::operator<<=(real angle) {
        this->rotation_ -= angle;
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::operator>>=(real angle) {
        this->rotation_ += angle;
        return static_cast<Derived &>(*this);
    }
}
