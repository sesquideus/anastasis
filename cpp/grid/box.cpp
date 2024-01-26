#include "box.h"

Pixel Box::as_pixel() const {
    /** Represent this box as a Pixel class for computation of intersections and such
     *
     */
    return Pixel(
            {static_cast<real>(this->left_),  static_cast<real>(this->bottom_)},
            {static_cast<real>(this->right_), static_cast<real>(this->bottom_)},
            {static_cast<real>(this->left_),  static_cast<real>(this->top_)},
            {static_cast<real>(this->right_), static_cast<real>(this->top_)}
    );
}

real Box::overlap(const Box & other) const {
    real hc = ((this->right_ - this->left_) - (other.right_ - other.left_)) / 2;
}
