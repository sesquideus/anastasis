#include "grid.h"

Grid::Grid(Point centre,
           std::pair<unsigned int, unsigned int> grid_size,
           std::pair<real, real> physical_size,
           real rotation,
           std::pair<real, real> pixfrac):
    centre_(centre),
    size_w_(grid_size.first),
    size_h_(grid_size.second),
    phys_w_(physical_size.first),
    phys_h_(physical_size.second),
    rotation_(rotation),
    pixfrac_x_(pixfrac.first),
    pixfrac_y_(pixfrac.second),
    pixel_width_(phys_w_ / static_cast<real>(size_w_)),
    pixel_height_(phys_h_ / static_cast<real>(size_h_))
{ }

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

Eigen::SparseMatrix<real> Grid::onto_canonical(const CanonicalGrid & canonical) const {
    std::vector<Overlap4D> active_pixels;

    // There will be about four times as much overlaps as there are model pixels
    active_pixels.reserve(4 * canonical.size());
    // Allocate the output matrix
    Eigen::SparseMatrix<real> matrix(canonical.width() * canonical.height(), this->size_w_ * this->size_h_);
    matrix.reserve(4 * canonical.size());

    for (int i = 0; i < this->size_w_; ++i) {
        for (int j = 0; j < this->size_h_; ++j) {
            auto p = this->world_pixel(i, j).bounding_box();
            //fmt::print("{} {} {} {} {} {}\n", this->width(), this->height(), p.left(), p.right(), p.bottom(), p.top());

            for (int x = std::max(0, p.left()); x < std::min(canonical.width(), p.right()); ++x) {
                for (int y = std::max(0, p.bottom()); y < std::min(canonical.height(), p.top()); ++y) {
                    real overlap = this->world_pixel(i, j).overlap(CanonicalGrid::pixel(x, y));
                    //fmt::print("{} {} -> {}\n", this->world_pixel(i, j), CanonicalGrid::pixel(x, y), overlap);
                    // Add as an active pixel if the overlap is larger than some very small value
                    if (overlap > Grid::NegligibleOverlap) {
                        active_pixels.emplace_back(x, y, i, j, overlap);
                    }
                }
            }
        }
    }

    for (auto && pixel: active_pixels) {
        matrix.insert(
            canonical.width() * std::get<1>(pixel) + std::get<0>(pixel),
            this->width() * std::get<3>(pixel) + std::get<2>(pixel)
        ) = std::get<4>(pixel);
    }
    matrix.makeCompressed();
    return matrix;
}

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
    this->phys_w_ *= scale;
    this->phys_h_ *= scale;
    this->pixel_width_ *= scale;
    this->pixel_height_ *= scale;
    return *this;
}

Grid & Grid::operator/=(real scale) {
    this->phys_w_ /= scale;
    this->phys_h_ /= scale;
    this->pixel_width_ /= scale;
    this->pixel_height_ /= scale;
    return *this;
}

Eigen::SparseMatrix<real> vstack(const std::vector<Eigen::SparseMatrix<real>> & matrices) {
    long rows = 0;
    long cols = 0;
    long nonzeros = 0;
    for (auto && matrix: matrices) {
        if ((rows > 0) && (matrix.cols() != cols)) {
            throw std::runtime_error("Sparse matrix width does not match");
        }
        cols = matrix.cols();
        rows += matrix.rows();
        nonzeros += matrix.nonZeros();
    }

    std::vector<Eigen::Triplet<real>> triplets;
    Eigen::Index base = 0;
    for (auto && matrix: matrices) {
        for (Eigen::Index c = 0; c < matrix.outerSize(); ++c) {
            for (Eigen::SparseMatrix<real>::InnerIterator it(matrix, c); it; ++it) {
                triplets.emplace_back(it.row() + base, it.col(), it.value());
            }
        }
        base += matrix.rows();
    }

    Eigen::SparseMatrix<real> m(rows, cols);
    m.reserve(nonzeros);
    fmt::print("{}×{} nonzero {}\n", cols, rows, nonzeros);

    m.setFromTriplets(triplets.begin(), triplets.end());
    return m;
}

Eigen::SparseMatrix<real> hstack(const std::vector<Eigen::SparseMatrix<real>> & matrices) {
    long rows = 0;
    long cols = 0;
    long nonzeros = 0;
    for (auto && matrix:matrices) {
        if ((cols > 0) && (matrix.rows() != rows)) {
            throw std::runtime_error("Sparse matrix height does not match");
        }
        rows = matrix.rows();
        cols += matrix.cols();
        nonzeros += matrix.nonZeros();
    }
    fmt::print("Joining {} matrices\n", matrices.size());

    std::vector<Eigen::Triplet<real>> triplets;
    Eigen::Index base = 0;
    for (auto && matrix: matrices) {
        fmt::print("{} nonzeros in matrix sum {}\n", matrix.nonZeros(), matrix.sum());
        for (Eigen::Index c = 0; c < matrix.outerSize(); ++c) {
            for (Eigen::SparseMatrix<real>::InnerIterator it(matrix, c); it; ++it) {
                triplets.emplace_back(it.row(), it.col() + base, it.value());
            }
        }
        base += matrix.cols();
    }
    Eigen::SparseMatrix<real> m(rows, cols);
    m.reserve(nonzeros);
    fmt::print("{}×{} nonzero {}\n", cols, rows, nonzeros);

    m.setFromTriplets(triplets.begin(), triplets.end());
    return m;
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