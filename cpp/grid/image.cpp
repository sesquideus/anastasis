#include "image.h"


Image::Image(int width, int height):
    width_(width),
    height_(height),
    data_()
{
}

Image & Image::map_in_place(const std::function<real(real)> & function) {
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            (*this)[col, row] = function((*this)[col, row]);
        }
    }
    return *this;
}

real Image::map_reduce(const std::function<real(real)> & map,
                       const std::function<real(real, real)> & reduce,
                       const real init) {
    real value = init;
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            value = reduce(value, map((*this)[col, row]));
        }
    }
    return value;
}