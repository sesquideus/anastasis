#ifndef ANASTASIS_CPP_MODELIMAGE_H
#define ANASTASIS_CPP_MODELIMAGE_H

#include <vector>
#include "grid/grid.h"
#include "grid/pixel/pixel.h"
#include "grid/detectorimage.h"


class DetectorImage;


class ModelImage {
private:
    const int width_;
    const int height_;
    Matrix data_;
public:
    explicit ModelImage(int width, int height);

    [[nodiscard]] Pixel pixel(int x, int y) const;
    [[nodiscard]] std::vector<std::vector<Pixel>> coordinates() const;

    [[nodiscard]] inline int width() const { return this->width_; }
    [[nodiscard]] inline int height() const { return this->height_; }
    [[nodiscard]] inline int size() const { return this->width_ * this->height_; }

    void drizzle(const std::vector<DetectorImage> & images);
    void operator+=(const DetectorImage & image);
};

#endif //ANASTASIS_CPP_MODELIMAGE_H
