#ifndef ANASTASIS_CPP_IMAGE_H
#define ANASTASIS_CPP_IMAGE_H

#include <functional>
#include <fstream>

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


class Image: public virtual AbstractGrid {
private:
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

    [[nodiscard]] Matrix & data() { return this->data_; }
    [[nodiscard]] const Matrix & data() const { return this->data_; }
    [[nodiscard]] inline real operator[](int x, int y) const { return this->data_(x, y); };
    [[nodiscard]] inline real & operator[](int x, int y) { return this->data_(x, y); };

    void save_raw(const std::string & filename) const;
    void save_npy(const std::string & filename) const;
    void save_bmp(const std::string & filename) const;
};


#endif //ANASTASIS_CPP_IMAGE_H
