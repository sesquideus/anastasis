#ifndef ANASTASIS_CPP_BOX_H
#define ANASTASIS_CPP_BOX_H

struct Box {
    int left;
    int right;
    int bottom;
    int top;
};

class Boxx {
private:
    int left_;
    int right_;
    int bottom_;
    int top_;
public:
    explicit Boxx(int left, int right, int bottom, int top):
        left_(left), right_(right), bottom_(bottom), top_(top) {}

    [[nodiscard]] inline int left() const { return this->left_; }
    [[nodiscard]] inline int right() const { return this->right_; }
    [[nodiscard]] inline int bottom() const { return this->bottom_; }
    [[nodiscard]] inline int top() const { return this->top_; }
};


#endif //ANASTASIS_CPP_BOX_H
