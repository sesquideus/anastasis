#ifndef ANASTASIS_CPP_ARRAY2D_H
#define ANASTASIS_CPP_ARRAY2D_H

#include <vector>

template<class T>
class Array2D {
private:
    std::vector<std::vector<T>> data;
public:
    T operator[](std::size_t r, std::size_t c) const { return this->data[r][c]; };
};


#endif //ANASTASIS_CPP_ARRAY2D_H
