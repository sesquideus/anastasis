#ifndef ANASTASIS_CPP_IMAGE_H
#define ANASTASIS_CPP_IMAGE_H

#include <functional>

#include "types/types.h"

class Image {
private:
    const int width_;
    const int height_;
    Matrix data_;
protected:
    Image map(const std::function<real(real)> & function);

    Image & map_in_place(const std::function<real(real)> & function);
    real map_reduce(const std::function<real(real)> & map,
                    const std::function<real(real, real)> & reduce = std::plus<>(),
                    real init = 0);
public:
    Image(int width, int height);
    explicit Image(const Matrix & data);

    Matrix & data() { return this->data_; }
    [[nodiscard]] const Matrix & data() const { return this->data_; }
    [[nodiscard]] inline real operator[](int x, int y) const { return this->data_(x, y); };
    [[nodiscard]] inline real & operator[](int x, int y) { return this->data_(x, y); };

    [[nodiscard]] inline int width() const { return this->width_; }
    [[nodiscard]] inline int height() const { return this->height_; }
    [[nodiscard]] inline pair<int> size() const { return {this->width_, this->height_}; }
};


#endif //ANASTASIS_CPP_IMAGE_H
