#include "image.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace Astar {
    template<class Derived>
    Image<Derived>::Image(int width, int height):
        AbstractGrid(width, height),
        data_(height, width)
    {
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                this->data_(row, col) = 0;
            }
        }
    }

    template<class Derived>
    Image<Derived>::Image(pair<int> size):
        AbstractGrid(size.first, size.second)
    {}

    template<class Derived>
    Image<Derived>::Image(const Matrix & data):
        AbstractGrid(data.cols(), data.rows()),
        data_(data)
    {}

    template<class Derived>
    Image<Derived>::Image(const std::string & filename):
        AbstractGrid(read_bitmap_header(filename))
    {
        std::ifstream bitmap_file;
        unsigned short planes, bpp;
        unsigned char value;
        int offset;
        int width = this->width();
        int height = this->height();

        bitmap_file.open(filename);
        bitmap_file.seekg(10, std::ios::beg);
        bitmap_file.read((char *) &offset, 4);
        bitmap_file.seekg(12, std::ios::cur);
        bitmap_file.read((char *) &planes, 2);
        if (planes != 0x0001) {
            throw std::runtime_error(fmt::format("Invalid number of image planes {}, must be 1", planes));
        }
        bitmap_file.read((char *) &bpp, 2);
        if (bpp != 0x0008) {
            throw std::runtime_error(fmt::format("Invalid BPP {}, must be 8", bpp));
        }
        bitmap_file.seekg(offset, std::ios::beg);

        this->data_.resize(height, width);
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                bitmap_file.read((char *) &value, 1);
                this->data_(row, col) = static_cast<real>(value) / 255.0;
            }
        }
        bitmap_file.close();
        fmt::print("Loaded bitmap '{}' with size {} × {}\n", filename, width, height);
    }

    template<class Derived>
    pair<int> Image<Derived>::read_bitmap_header(const std::string & filename) {
        std::ifstream bitmap_file;
        unsigned short header;
        int width, height;

        bitmap_file.open(filename);
        if (!bitmap_file.is_open()) {
            throw std::runtime_error(fmt::format("Could not open file {}", filename));
        }
        bitmap_file.seekg(0, std::ios::beg);
        bitmap_file.read((char *) &header, 2);
        if (header != 0x4D42) {
            throw std::runtime_error(fmt::format("Invalid BMP magic value {:04x}", header));
        }
        bitmap_file.seekg(18, std::ios::beg);
        bitmap_file.read((char *) &width, 4);
        bitmap_file.read((char *) &height, 4);
        bitmap_file.close();
        return {width, height};
    }

    template<class Derived>
    Derived Image<Derived>::map(const std::function<real(real)> & function) {
        auto image = *this;
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                image[col, row] = function((*this)[col, row]);
            }
        }
        return image;
    }

    template<class Derived>
    Derived & Image<Derived>::fill(const real value) {
        this->map_in_place([=]() { return value; });
    }

    template<class Derived>
    Derived & Image<Derived>::map_in_place(const std::function<real()> & function) {
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                (*this)[col, row] = function();
            }
        }
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & Image<Derived>::map_in_place(const std::function<real(real &)> & function) {
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                function((*this)[col, row]);
            }
        }
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & Image<Derived>::map_in_place(const std::function<real(int, int, real &)> & function) {
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                function(col, row, (*this)[col, row]);
            }
        }
        return static_cast<Derived &>(*this);
    }

    template<class Derived>
    Derived & Image<Derived>::operator*=(real value) {
        return this->map_in_place([&](real & x) { return x *= value; });
    }

    /** Divide every pixel by `value` **/
    template<class Derived>
    Derived & Image<Derived>::operator/=(real value) {
        return this->map_in_place([&](real & x) { return x /= value; });
    }

    template<class Derived>
    real Image<Derived>::map_reduce(const std::function<real(real)> & map,
                           const std::function<real(real, real)> & reduce,
                           const real init) const {
        real value = init;
        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                value = reduce(value, map((*this)[col, row]));
            }
        }
        return value;
    }

    template<class Derived>
    Derived & Image<Derived>::zero() {
        return this->map_in_place([]() { return 0; });
    }

    template<class Derived>
    Derived & Image<Derived>::randomize() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::weibull_distribution<> weibull;

        return this->map_in_place([&]() { return weibull(gen); });
    }


    /**
     * Save the image to a raw file of sequential real numbers with no size data. Very crude.
     * @param filename
     */
    template<class Derived>
    void Image<Derived>::save_raw(const std::string & filename) const {
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
     * Save the image to a raw file of sequential real numbers as a numpy array.
     * Only implements the very minimum to work, feel free to extend.
     * @param filename
     */
    template<class Derived>
    void Image<Derived>::save_npy(const std::string & filename) const {
        std::ofstream out;
        out.open(filename);
        unsigned char c = 0x93;
        out.write(reinterpret_cast<const char *>(&c), sizeof c);
        out.write("NUMPY", 5);
        unsigned short d = 1;
        out.write(reinterpret_cast<const char *>(&d), sizeof d);
        d = 0x76;
        out.write(reinterpret_cast<const char *>(&d), sizeof d);
        std::string desc = fmt::format("{{'descr': '<f{}', 'fortran_order': False, 'shape': ({}, {})}}",
                                       sizeof(real), this->height(), this->width());
        out.write(desc.c_str(), desc.length());
        c = 0x20;
        // The length of the header must be a multiple of 64, so it is padded with spaces
        for (int i = 10 + desc.length(); i % 64 != 63; ++i) {
            out.write(reinterpret_cast<const char *>(&c), sizeof c);
        }
        c = 0x0A;
        out.write(reinterpret_cast<const char*>(&c), sizeof c);

        for (int row = 0; row < this->height(); ++row) {
            for (int col = 0; col < this->width(); ++col) {
                real value = (*this)[col, row];
                out.write(reinterpret_cast<const char *>(&value), sizeof value);
            }
        }
        out.close();
        fmt::print("Image with size {} saved to {}\n", this->size(), filename);
    }

    /**
     * Save an image to a bitmap file with BMPHEADERINFO. Crude and inefficient but gets the job done.
     * @param filename
     */
    template<class Derived>
    void Image<Derived>::save_bmp(const std::string & filename) const {
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
                value = col < this->width() ? static_cast<char>(trim((*this)[col, row]) * 255) : 0;
                out.write(reinterpret_cast<char *>(&value), 1);
            }
        }
        fmt::print("Saved to {}\n", filename);
        out.close();
    }

    template<class Derived>
    real Image<Derived>::minimum() const {
        return this->map_reduce([](real x) { return std::abs<real>(x); }, [](real x, real y) { return x < y ? x : y; });
    }

    template<class Derived>
    real Image<Derived>::maximum() const {
        return this->map_reduce([](real x) { return std::abs<real>(x); }, [](real x, real y) { return x > y ? x : y; });
    }

    template<class Derived>
    real Image<Derived>::rms() const {
        return std::sqrt(this->map_reduce(
            [](real x) { return x * x; }
        ) / static_cast<real>(this->count()));
    }
}
