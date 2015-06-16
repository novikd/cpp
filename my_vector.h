//
// Created by 1 on 15.06.2015.
//

#include <c++/bits/shared_ptr.h>
#include <vector>

#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#endif //BIGINT_MY_VECTOR_H

struct my_vector {
    void push_back(size_t val);
    size_t pop_back();
    size_t back() const;
    size_t operator[] (int ind);
    size_t size() const;
    void resize(size_t len);
    void resize(size_t len, size_t val);
private:
    std::shared_ptr<std::vector<size_t>> data;
    bool sign;
};
