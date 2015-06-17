//
// Created by 1 on 15.06.2015.
//

#include <c++/bits/shared_ptr.h>
#include <vector>
#include <stddef.h>

#ifndef BIGINT_MY_VECTOR_H
#define BIGINT_MY_VECTOR_H

#endif //BIGINT_MY_VECTOR_H

union saved_data {
    size_t* val;
    std::vector<size_t>* vals;
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
    size_t& operator[] (const size_t index);

private:
    std::shared_ptr<saved_data> data;
    bool is_small;
    void correct();
    void make_own();
    void to_array();
};

size_t& operator[] (const size_t index);