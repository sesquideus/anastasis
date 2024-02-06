#include "grid.h"
#include <iostream>

Grid::Grid(Point centre,
           pair<int> grid_size,
           pair<real> physical_size,
           real rotation,
           pair<real> pixfrac):
    size_w_(grid_size.first),
    size_h_(grid_size.second),
    centre_(centre),
    phys_w_(physical_size.first),
    phys_h_(physical_size.second),
    rotation_(rotation),
    pixfrac_x_(pixfrac.first),
    pixfrac_y_(pixfrac.second),
    pixel_width_(phys_w_ / static_cast<real>(size_w_)),
    pixel_height_(phys_h_ / static_cast<real>(size_h_))
{ }

Grid Grid::from_pixel_size(Point centre,
                           pair<int> grid_size,
                           pair<real> pixel_size,
                           real rotation,
                           pair<real> pixfrac) {
    return Grid(centre, grid_size,
                {
                    pixel_size.first * static_cast<real>(grid_size.first),
                    pixel_size.second * static_cast<real>(grid_size.second)
                }, rotation, pixfrac);
}

Point Grid::grid_centre(unsigned int x, unsigned int y) const {
    real lx = (static_cast<real>(x) + 0.5) / static_cast<real>(this->size_w_) * this->phys_w_ - this->phys_w_ * 0.5;
    real ly = (static_cast<real>(y) + 0.5) / static_cast<real>(this->size_h_) * this->phys_h_ - this->phys_h_ * 0.5;
    return Point(lx, ly);
}

Point Grid::world_centre(unsigned int x, unsigned int y) const {
    Point grid_centre = this->grid_centre(x, y);
    real sina = std::sin(this->rotation_);
    real cosa = std::cos(this->rotation_);
    return Point(grid_centre.x * cosa - grid_centre.y * sina, grid_centre.x * sina + grid_centre.y * cosa);
}

Pixel Grid::grid_pixel(unsigned int x, unsigned int y) const {
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

Pixel Grid::world_pixel(unsigned int x, unsigned int y) const {
    Pixel raw = this->grid_pixel(x, y);
    return Pixel(
        this->centre_ + raw.a().rotated(this->rotation_),
        this->centre_ + raw.b().rotated(this->rotation_),
        this->centre_ + raw.d().rotated(this->rotation_),
        this->centre_ + raw.c().rotated(this->rotation_)
    );
}

/*
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
}

** Compute the overlap as a 2D matrix (huge size, even if sparse) **
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
}*/

void Grid::print() const {
    fmt::print("Grid at {}, size {:d}×{:d}, extent {:.6f} {:.6f}, rotation {:.6f}, pixfrac {:.3f}×{:.3f}\n",
               this->centre_, this->size_w_, this->size_h_, this->phys_w_, this->phys_h_,
               this->rotation_, this->pixfrac_x_, this->pixfrac_y_);
}

void Grid::print_world() const {
    Point bottomleft = this->world_pixel(0, 0).a();
    Point bottomright = this->world_pixel(this->width() - 1, 0).b();
    Point topleft = this->world_pixel(0, this->height() - 1).d();
    Point topright = this->world_pixel(this->width() - 1, this->height() - 1).c();
    fmt::print("Grid with world coordinates spanning {} {} {} {}\n",
               bottomleft, bottomright, topleft, topright
    );
}

Grid & Grid::operator+=(Point shift) {
    this->centre_ += shift;
    return *this;
}

Grid & Grid::operator-=(Point shift) {
    this->centre_ -= shift;
    return *this;
}

Grid & Grid::operator*=(real scale) {
    *this *= {scale, scale};
    return *this;
}

Grid & Grid::operator*=(pair<real> scale) {
    this->phys_w_ *= scale.first;
    this->phys_h_ *= scale.second;
    this->pixel_width_ *= scale.first;
    this->pixel_height_ *= scale.second;
    return *this;
}

Grid & Grid::operator/=(real scale) {
    *this /= {scale, scale};
    return *this;
}

Grid & Grid::operator/=(pair<real> scale) {
    this->phys_w_ /= scale.first;
    this->phys_h_ /= scale.second;
    this->pixel_width_ /= scale.first;
    this->pixel_height_ /= scale.second;
    return *this;
}

Eigen::SparseMatrix<real> stack(const std::vector<Eigen::SparseMatrix<real>> & matrices, bool vertical) {
    /**
     * Stack a vector of matrices
     */
    long across = 0;
    long along = 0;
    long nonzeros = 0;
    for (auto && matrix: matrices) {
        long num_along = vertical ? matrix.rows() : matrix.cols();
        long num_across = vertical ? matrix.cols() : matrix.rows();
        if (along > 0) {
           assert((num_across == across) && "Across-concatenation dimensions do not match");
        }
        across = num_across;
        along += num_along;
        nonzeros += matrix.nonZeros();
    }

   // fmt::print("Joining {} matrices along {} axis\n", matrices.size(), vertical ? "vertical" : "horizontal");
    std::vector<Eigen::Triplet<real>> triplets;
    triplets.reserve(nonzeros);

    Eigen::Index base = 0;
    for (auto && matrix: matrices) {
        for (Eigen::Index c = 0; c < matrix.outerSize(); ++c) {
            for (Eigen::SparseMatrix<real>::InnerIterator it(matrix, c); it; ++it) {
                triplets.emplace_back(
                    vertical ? it.row() + base : it.row(),
                    vertical ? it.col() : base + it.col(),
                    it.value()
                );
            }
        }
        base += vertical ? matrix.rows() : matrix.cols();
    }

    long cols = vertical ? across : along;
    long rows = vertical ? along : across;

    Eigen::SparseMatrix<real> m(rows, cols);
    m.reserve(nonzeros);
    fmt::print("After stacking: {}×{} matrix with {} nonzero elements\n", cols, rows, nonzeros);

    m.setFromTriplets(triplets.begin(), triplets.end());
    return m;
}

Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices) {
    return stack(matrices, true);
}

Eigen::SparseMatrix<real> hstack(const std::vector<Eigen::SparseMatrix<real>> & matrices) {
    return stack(matrices, false);
}


Eigen::SparseMatrix<real> vstack2(std::vector<Eigen::SparseMatrix<real>> matrices) {
    long rows = 0;
    long cols = 0;
    long nonzeros = 0;
    for (auto & matrix: matrices) {
        if ((rows > 0) && (matrix.cols() != cols)) {
            throw std::runtime_error("Sparse matrix width does not match");
        }
        cols = matrix.cols();
        rows += matrix.rows();
        nonzeros += matrix.nonZeros();
    }

    Eigen::SparseMatrix<real> m(rows, cols);
    m.reserve(nonzeros);
    fmt::print("{} {}\n", cols, nonzeros);

    for (Eigen::Index c = 0; c < cols; ++c) {
        Eigen::Index base = 0;
        for (auto && matrix: matrices) {
            m.startVec(c);
            for (Eigen::SparseMatrix<real>::InnerIterator it(matrix, c); it; ++it) {
                matrix.startVec(c);
                fmt::print("s {} {}\n", it, c);
                m.insertBack(base + it.row(), c) = it.value();
                fmt::print("e {} {}\n", it, c);
            }
            base += matrix.rows();
        }
    }
    m.finalize();
    return m;
}

Grid operator+(const Grid & grid, Point shift) {
    auto new_grid = grid;
    new_grid += shift;
    return new_grid;
}
