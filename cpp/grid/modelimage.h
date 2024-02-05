#ifndef ANASTASIS_CPP_MODELIMAGE_H
#define ANASTASIS_CPP_MODELIMAGE_H

#include <vector>
#include "grid/grid.h"
#include "grid/pixel/pixel.h"
#include "grid/detectorimage.h"


class DetectorImage;


/** ModelImage models a finale product of the data reduction.
 *  Its pixels are always equally sized and placed on an aligned orthogonal grid.
 */
class ModelImage {
private:
    const int width_;
    const int height_;
    Matrix data_;
    Matrix noise_;
public:
    explicit ModelImage(int width, int height);

    [[nodiscard]] Pixel pixel(int x, int y) const;
    [[nodiscard]] inline real operator[](int x, int y) const { return this->data_(x, y); };
    [[nodiscard]] inline real & operator[](int x, int y) { return this->data_(x, y); };

    [[nodiscard]] inline int width() const { return this->width_; }
    [[nodiscard]] inline int height() const { return this->height_; }
    [[nodiscard]] inline int size() const { return this->width_ * this->height_; }

    const Matrix & data() const { return this->data_; }
    Matrix & data() { return this->data_; }

    void drizzle(const std::vector<DetectorImage> & images);
    ModelImage & operator+=(const DetectorImage & image);

    // Find the mean square difference between the pictures, expressed as a number between 0 and 1

    real kullback_leibler(const ModelImage & other) const;
    real squared_difference(const ModelImage & other) const;
    real operator^(const ModelImage & other) const;

    real total_flux() const;
    void save(const std::string & filename) const;
};

#endif //ANASTASIS_CPP_MODELIMAGE_H
