//
// Created by 1 on 15.06.2015.
//

#include <c++/bits/shared_ptr.h>
#include <vector>

#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#endif //BIGINT_MY_VECTOR_H

union saved_data {
    std::shared_ptr<size_t> val;
    std::shared_ptr<std::vector<size_t>> vals;
};

struct my_vector {
    my_vector();
    my_vector(size_t val);
    my_vector(std::vector<size_t> val);
    ~my_vector();
    void push_back(size_t val);
    size_t pop_back();
    size_t back() const;
    size_t size() const;
    void resize(const size_t len);
    void resize(const size_t len, size_t val);
    size_t& operator[] (my_vector& v, const size_t index);

private:
    saved_data data;
    bool sign, is_small;
    void correct();
    void make_own();
};

size_t& operator[] (my_vector& v, const size_t index);