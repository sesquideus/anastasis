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
        {this->centre_.x + raw.corners()[0][0].rotated(this->rotation_).x, this->centre_.y + raw.corners()[0][0].rotated(this->rotation_).y},
        {this->centre_.x + raw.corners()[0][1].rotated(this->rotation_).x, this->centre_.y + raw.corners()[0][1].rotated(this->rotation_).y},
        {this->centre_.x + raw.corners()[1][0].rotated(this->rotation_).x, this->centre_.y + raw.corners()[1][0].rotated(this->rotation_).y},
        {this->centre_.x + raw.corners()[1][1].rotated(this->rotation_).x, this->centre_.y + raw.corners()[1][1].rotated(this->rotation_).y}
    );
}

Eigen::SparseMatrix<real> Grid::onto_canonical(const CanonicalGrid & canonical) const {
    std::vector<std::tuple<std::size_t, std::size_t, std::size_t, std::size_t, real>> active_pixels;
    for (std::size_t i = 0; i < this->size_w_; ++i) {
        for (std::size_t j = 0; j < this->size_h_; ++j) {
            auto p = this->world_pixel(i, j).bounding_box();
            fmt::print("{} {} {} {} {} {}\n", this->width(), this->height(), p.left(), p.right(), p.bottom(), p.top());

            for (std::size_t x = std::max(0, p.bottom()); x < std::min(canonical.width(), p.top()); ++x) {
                for (std::size_t y = std::max(0, p.left()); y < std::min(canonical.height(), p.right()); ++y) {
                    real overlap = this->world_pixel(i, j).overlap(canonical.pixel(x, y));
                  //  fmt::print("{} {} {} {}\n", i, j, x, y);
                    if (overlap > 0) {
                        active_pixels.emplace_back(i, j, x, y, overlap);
                    }
                }
            }
        }
    }

    Eigen::SparseMatrix<real> matrix(this->size_w_ * this->size_h_, canonical.width() * canonical.height());
    for (auto && pixel: active_pixels) {
        matrix.insert(
            this->width() * std::get<1>(pixel) + std::get<0>(pixel),
            canonical.width() * std::get<3>(pixel) + std::get<2>(pixel)
        ) = std::get<4>(pixel);
    }
    matrix.makeCompressed();

    return matrix;
}

void Grid::print() const {
    fmt::print("Grid at {:.6f} {:.6f}, size {:d}×{:d}, extent {:.6f} {:.6f}, rotation {:.6f}, pixfrac {:.3f}×{:.3f}\n",
               this->centre_.x, this->centre_.y, this->size_w_, this->size_h_, this->phys_w_, this->phys_h_,
               this->rotation_, this->pixfrac_x_, this->pixfrac_y_);
}

void Grid::print_world() const {
    Point bottomleft = this->world_pixel(0, 0).corners()[0][0];
    Point bottomright = this->world_pixel(0, 1).corners()[0][1];
    Point topleft = this->world_pixel(0, 0).corners()[0][0];
    Point topright = this->world_pixel(0, 1).corners()[0][1];
    fmt::print("Grid with world coordinates spanning ({} {}) ({} {}) ({} {}) ({} {})",
               bottomleft.x, bottomleft.y,
               bottomright.x, bottomright.y,
               topleft.x, topleft.y,
               topright.x, topright.y
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
    return *this;
}

Grid & Grid::operator/=(real scale) {
    this->phys_w_ *= scale;
    this->phys_h_ *= scale;
    return *this;
}