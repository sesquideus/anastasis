#ifndef ANASTASIS_CPP_MODELIMAGE_H
#define ANASTASIS_CPP_MODELIMAGE_H

#include <vector>
#include "grid/image.h"
#include "grid/grid.h"
#include "grid/pixel/pixel.h"
#include "grid/detectorimage.h"


class DetectorImage;


/** ModelImage models a finale product of the data reduction.
 *  Its pixels are always equally sized and placed on an aligned orthogonal grid.
 */
class ModelImage: public Image {
private:
    Matrix noise_;
public:
    explicit ModelImage(int width, int height);

    [[nodiscard]] Pixel pixel(int x, int y) const;

    void drizzle(const std::vector<DetectorImage> & images);
    ModelImage & operator+=(const DetectorImage & image);

    // Find the mean square difference between the pictures, expressed as a number between 0 and 1

    real kullback_leibler(const ModelImage & other) const;
    real squared_difference(const ModelImage & other) const;
    real operator^(const ModelImage & other) const;

    real total_flux() const;
    void save_raw(const std::string & filename) const;
    void save_bmp(const std::string & filename) const;
};

#endif //ANASTASIS_CPP_MODELIMAGE_H
