#include "placedgrid.h"
#include <iostream>

namespace Astar {
    template<class Derived>
    PlacedGrid<Derived>::PlacedGrid(Point centre,
                           pair<int> grid_size,
                           pair<real> physical_size,
                           real rotation,
                           pair<real> pixfrac):
        AbstractGrid(grid_size),
        centre_(centre),
        phys_w_(physical_size.first),
        phys_h_(physical_size.second),
        rotation_(rotation),
        pixfrac_x_(pixfrac.first),
        pixfrac_y_(pixfrac.second),
        pixel_width_(pixfrac_x_ * phys_w_ / static_cast<real>(grid_size.first)),
        pixel_height_(pixfrac_y_ * phys_h_ / static_cast<real>(grid_size.second))
    { }

    template<class Derived>
    PlacedGrid<Derived> PlacedGrid<Derived>::from_pixel_size(Point centre,
                                                             pair<int> grid_size,
                                                             pair<real> pixel_size,
                                                             real rotation,
                                                             pair<real> pixfrac) {
        real width = pixel_size.first * static_cast<real>(grid_size.first);
        real height = pixel_size.second * static_cast<real>(grid_size.second);
        return {centre, grid_size, {width, height}, rotation, pixfrac};
    }

    template<class Derived>
    Point PlacedGrid<Derived>::grid_centre(unsigned int x, unsigned int y) const {
        real lx = (static_cast<real>(x) + 0.5) / static_cast<real>(this->width()) * this->phys_w_ - this->phys_w_ * 0.5;
        real ly = (static_cast<real>(y) + 0.5) / static_cast<real>(this->height()) * this->phys_h_ - this->phys_h_ * 0.5;
        return {lx, ly};
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
        Point grid_centre = this->grid_centre(x, y);
        real hw = this->pixel_width_ * this->pixfrac_x_ * 0.5;
        real hh = this->pixel_height_ * this->pixfrac_y_ * 0.5;
        return Pixel(
            {grid_centre.x - hw, grid_centre.y - hh},
            {grid_centre.x + hw, grid_centre.y - hh},
            {grid_centre.x - hw, grid_centre.y + hh},
            {grid_centre.x + hw, grid_centre.y + hh}
        );
    }

    template<class Derived>
    Pixel PlacedGrid<Derived>::world_pixel(unsigned int x, unsigned int y) const {
        Pixel raw = this->grid_pixel(x, y);
        return Pixel(
            this->centre_ + raw.a().rotated(this->rotation_),
            this->centre_ + raw.b().rotated(this->rotation_),
            this->centre_ + raw.d().rotated(this->rotation_),
            this->centre_ + raw.c().rotated(this->rotation_)
        );
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::set_centre(Point centre) {
        this->centre_ = centre;
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & PlacedGrid<Derived>::set_physical_size(pair<real> size) {
        this->phys_w_ = size.first;
        this->phys_h_ = size.second;
        this->pixel_width_ = this->pixfrac_x_ * this->phys_w_ / static_cast<real>(this->width());
        this->pixel_height_ = this->pixfrac_y_ * this->phys_h_ / static_cast<real>(this->height());
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
    Derived & PlacedGrid<Derived>::transform(const AffineTransformation & transform) {
        this->centre_ += transform.translation();
        this->rotation_ += 0;
        return static_cast<Derived &>(*this);
    }

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
