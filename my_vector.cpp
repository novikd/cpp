//
// Created by 1 on 15.06.2015.
//

#include "my_vector.h"

my_vector::my_vector(size_t val) {
    *this->data->val = val;
}

my_vector::my_vector()
        :my_vector(0)
{}

my_vector::my_vector(std::vector<size_t> val) {
    *this->data->vals = val;
}

my_vector::~my_vector() {
    delete this->data;
}

void my_vector::correct() {
    if (!this->is_small && this->data->vals->size() == 1) {
        size_t* curr = new *this->data->vals->[0];
        delete this->data->vals;
        this->data->val = curr;
        this->is_small = true;
    }
}

void my_vector::make_own() {
    if (this->data.use_count() > 1) {
        if (this->is_small) {
            this->data->val = new size_t(*this->data->val);
        } else {
            std::vector<size_t>* curr = new std::vector<size_t>;
            for (size_t i = 0; i < this->data->vals->size(); ++i) {
                curr->push_back(this->data->vals->[i]);
            }
            this->data->vals = curr;
        }
    }
}

void my_vector::to_array() {
    this->make_own();
    if (this->is_small) {
        std::vector<size_t>* tmp = new std::vector<size_t>;
        tmp->push_back(*this->data->val);
        delete this->data->val;
        this->data->vals = tmp;
    }
}

void my_vector::push_back(size_t val) {
    this->make_own();
    if (this->is_small) {
        std::vector<size_t>* curr = new std::vector<size_t>;
        curr->push_back(*this->data->val);
        curr->push_back(val);
        this->is_small = false;
        delete this->data->val;
        this->data->vals = curr;
    } else {
        this->data->vals->push_back(val);
    }
}

size_t my_vector::pop_back() {
    this->make_own();
    size_t ans = this->data->vals->back();
    this->data->vals->pop_back();
    this->correct();
    return ans;
}

size_t my_vector::back() const {
    if (this->is_small) {
        return *this->data->val;
    } else {
        return this->data->vals->back();
    }
}

size_t my_vector::size() const {
    if (this->is_small) {
        return 1;
    } else {
        return this->data->vals->size();
    }
}

void my_vector::resize(size_t len) {
    this->to_array();
    this->data->vals->resize(len);
    this->correct();
}

void my_vector::resize(size_t len, size_t val) {
    this->to_array();
    this->data->vals->resize(len, val);
    this->correct();
}

size_t& my_vector::operator[] (const size_t index) {
    this->make_own();
    if (this->is_small) {
        return *this->data->val;
    } else {
        return *this->data->vals->[index];
    }
}