#ifndef ANASTASIS_CPP_MODELIMAGE_H
#define ANASTASIS_CPP_MODELIMAGE_H

#include <vector>
#include "grid/image.h"
#include "grid/placedgrid.h"
#include "grid/pixel/pixel.h"
#include "grid/detectorimage.h"


class DetectorImage;


/** ModelImage models a final product of the data reduction.
 *  Its pixels are always square, 1×1 in size and placed on an aligned orthogonal grid starting at [0, 0].
 *  They contain image data and are thus a subclass of Image
 */
class ModelImage: public Image<ModelImage> {
private:
    Matrix variance_;

    ModelImage & apply(const ModelImage & other, const std::function<real(real &, real)> & op);
public:
    explicit ModelImage(int width, int height);
    explicit ModelImage(pair<int> size);

    [[nodiscard]] Pixel pixel(int x, int y) const;

    ModelImage & naive_drizzle(const std::vector<DetectorImage> & images);
    ModelImage & naive_drizzle(const DetectorImage & image);
    ModelImage & weighted_drizzle(const std::vector<DetectorImage> & images);
    ModelImage & weighted_drizzle(const DetectorImage & image);
    //ModelImage & operator+=(const DetectorImage & image);
    ModelImage & operator+=(const ModelImage & other);
    ModelImage & operator-=(const ModelImage & other);

    [[nodiscard]] SparseMatrix overlap_matrix(const DetectorImage & image) const;

    // Find the mean square difference between the pictures, expressed as a number between 0 and 1
    [[nodiscard]] real dot_product(const ModelImage & other) const;
    [[nodiscard]] real squared_difference(const ModelImage & other) const;
    ModelImage operator-(const ModelImage & other) const;
    real operator*(const ModelImage & other) const;
    real operator%(const ModelImage & other) const;
    real operator^(const ModelImage & other) const;

    [[nodiscard]] real total_flux() const;
};

template<>
class fmt::formatter<ModelImage> {
public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> fmt::format_parse_context::iterator {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') return it;
        return it;
    }

    auto format(const ModelImage & image, format_context & ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "Model image with size {:d}×{:d}",
                              image.size().first, image.size().second);
    }
};

#endif //ANASTASIS_CPP_MODELIMAGE_H
