#ifndef ANASTASIS_CPP_IMAGE_H
#define ANASTASIS_CPP_IMAGE_H

#include <functional>
#include <fstream>
#include <random>

#include "types/types.h"
#include "utils/functions.h"
#include "abstractgrid.h"

#pragma pack(push,2)
typedef struct tagBITMAPHEADER {
    unsigned short  biHeader = 0x4D42;
    unsigned int    biSize = 0;
    unsigned short  biReservedCrap1 = 0;
    unsigned short  biReservedCrap2 = 0;
    unsigned int    biOffset = 1078;
} BITMAPHEADER;

typedef struct tagBITMAPINFOHEADER {
    unsigned int    biSize = 40;
    int             biWidth = 0;
    int             biHeight = 0;
    short           biPlanes = 1;
    short           biBitCount = 8;
    unsigned int    biCompression = 0;
    unsigned int    biSizeImage = 0;
    int             biXPelsPerMeter = 2835;
    int             biYPelsPerMeter = 2835;
    unsigned int    biClrUsed = 0;
    unsigned int    biClrImportant = 0;
} BITMAPINFOHEADER;
#pragma pack(pop)


namespace Astar {
    /** An Image is an abstract grid with no placement, but contains a piece of data at every pixel.
     *  Right now it is just for real numbers, but can be made into a template.
     */
    template<class Derived>
    class Image: public virtual AbstractGrid {
    protected:
        Matrix data_;

        static pair<int> read_bitmap_header(const std::string & filename);
        Derived map(const std::function<real(real)> & function);

        Derived & fill(real value);
        /** Apply a parameterless function to every pixel (constant, random, ...) **/
        Derived & map_in_place(const std::function<real()> & function);
        /** Apply a R -> R function to every pixel **/
        Derived & map_in_place(const std::function<real(real &)> & function);
        /** Apply a R -> R function to every pixel depending on pixel coordinates and current value **/
        Derived & map_in_place(const std::function<real(int, int, real &)> & function);

        real map_reduce(const std::function<real(real)> & map,
                        const std::function<real(real, real)> & reduce = std::plus<>(),
                        real init = 0) const;
    public:
        explicit Image(int width, int height);
        explicit Image(pair<int> size);
        explicit Image(const Matrix & data);
        explicit Image(const std::string & filename);

        [[nodiscard]] Matrix & data() { return this->data_; }
        [[nodiscard]] const Matrix & data() const { return this->data_; }
        [[nodiscard]] inline real operator[](int x, int y) const { return this->data_(y, x); };
        [[nodiscard]] inline real & operator[](int x, int y) { return this->data_(y, x); };

        Derived & randomize();

        Derived & operator*=(real value);
        Derived & operator/=(real value);

        void save_raw(const std::string & filename) const;
        void save_npy(const std::string & filename) const;
        void save_bmp(const std::string & filename) const;
    };

    template<class Derived> Derived operator*(Image<Derived> image, real value);
    template<class Derived> Derived operator/(Image<Derived> image, real value);

    template<class Derived>
    Derived operator*(Image<Derived> image, real value) {
        return image *= value;
    }

    template<class Derived>
    Derived operator/(Image<Derived> image, real value) {
        return image /= value;
    }
}

#include "grid/image.tpp"

#endif //ANASTASIS_CPP_IMAGE_H
