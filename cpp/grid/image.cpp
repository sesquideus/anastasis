#include "image.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>


Image::Image(int width, int height):
    width_(width),
    height_(height),
    data_(width, height)
{
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            this->data_(col, row) = 0;
        }
    }
}

Image::Image(const Matrix & data):
    width_(data.cols()),
    height_(data.rows()),
    data_(data)
{}

Image Image::map(const std::function<real(real)> & function) {
    auto image = *this;
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            image[col, row] = function(image[col, row]);
        }
    }
    return image;
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

/**
 * Save the image to a raw file of sequential real numbers with no size data. Very crude.
 * @param filename
 */
void Image::save_raw(const std::string & filename) const {
    std::ofstream out;
    out.open(filename);
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            real value = (*this)[col, row];
            out.write(reinterpret_cast<const char*>(&value), sizeof value);
        }
    }
    out.close();
}

/**
 * Save the image to a raw file of sequential real numbers with no size data. Very crude.
 * @param filename
 */
void Image::save_npy(const std::string & filename) const {
    std::ofstream out;
    out.open(filename);
    out.write()
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < this->width(); ++col) {
            real value = (*this)[col, row];
            out.write(reinterpret_cast<const char*>(&value), sizeof value);
        }
    }
    out.close();
}

/**
 * Save an image to a bitmap file with BMPHEADERINFO. Crude and inefficient but gets the job done.
 * @param filename
 */
void Image::save_bmp(const std::string & filename) const {
    std::ofstream out;
    out.open(filename, std::ios::out | std::ios::binary);

    BITMAPHEADER bitmapheader;
    unsigned int size = (this->width() + 3) / 4 * 4 * this->height();
    bitmapheader.biSize = 1078 + size;

    BITMAPINFOHEADER bitmapinfoheader;
    bitmapinfoheader.biWidth = this->width();
    bitmapinfoheader.biHeight = this->height();
    out.write((char *) &bitmapheader, 14);
    out.write((char *) &bitmapinfoheader, 40);

    for (int colour = 0; colour < 0x01000000; colour += 0x00010101) {
        out.write((char *) &colour, 4);
    }

    unsigned char value;
    for (int row = 0; row < this->height(); ++row) {
        for (int col = 0; col < (this->width() + 3) / 4 * 4; ++col) {
            value = col < this->width() ? static_cast<char>(trim((*this)[col, row], 0, 1) * 255) : 0;
            out.write((char *) &value, 1);
        }
    }
    fmt::print("Saved to {}\n", filename);
    out.close();
}

