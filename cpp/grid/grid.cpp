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

Eigen::SparseMatrix<real> Grid::onto_canonical(const CanonicalGrid & other) const {
    for (unsigned int i = 0; i < this->size_w_; ++i) {
        for (unsigned int j = 0; j < this->size_h_; ++j) {
            this->world_pixel(i, j).bounding_box();
        }
    }

    return Eigen::SparseMatrix<real>();
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

Pixel CanonicalGrid::pixel(unsigned int x, unsigned int y) {
    return Pixel(Point(x, y), Point(x + 1, y), Point(x + 1, y + 1), Point(x, y + 1));
}
