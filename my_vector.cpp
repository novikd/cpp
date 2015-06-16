//
// Created by 1 on 15.06.2015.
//

#include "my_vector.h"

void my_vector::push_back(size_t val) {
    if (this->is_small) {
        std::vector<size_t>* curr = new std::vector<size_t>;
        (*curr).push_back(*this->data.val);
        (*curr).push_back(val);
        this->is_small = false;
        delete this->data.val;
        this->data.vals = curr;
    } else {
        if (this->data.vals.use_count() == 1) {
            this->data.vals->push_back(val);
        } else {
            std::vector<size_t>* curr = new std::vector<size_t>;
            for (size_t i = 0; i < this->data.vals->size(); ++i) {
                curr->push_back((*this->data.vals)[i]);
            }
            curr->push_back(val);
            this->data.vals = curr;
        }
    }
}

my_vector::my_vector(size_t val) {
    *this->data.val = val;
}

my_vector::my_vector()
    :my_vector(0) { }

my_vector::my_vector(std::vector<size_t> val) {
    *this->data.vals = val;
}

my_vector::~my_vector() {
    delete this->data.vals;
}

size_t my_vector::pop_back() {
    if (this->data.vals.use_count() == 1) {
        size_t ans = this->data.vals->back();
        this->data.vals->pop_back();
        if (this->data.vals->size() == 1) {
            this->is_small = true;
            this->data.val = new ((*this->data.vals)[0]);
            delete this->data.vals;
        }
        return ans;
    } else {
        std::vector<size_t>* curr = new std::vector<size_t>;
        for (size_t i = 0; i < this->data.vals->size(); ++i) {
            curr->push_back((*this->data.vals)[i]);
        }
        size_t ans = curr->back();
        curr->pop_back();
        this->data.vals = curr;
        if (curr->size() == 1) {
            this->is_small = true;
            this->data.val = &(*curr)[0];
            delete curr;
        }
        return ans;
    }
}

size_t my_vector::back() const {
    if (this->is_small) {
        return *(this->data.val);
    } else {
        return *(this->data.vals->[0]);
    }
}

size_t my_vector::size() const {
    if (is_small) {
        return 1;
    } else {
        return this->data.vals->size();
    }
}

void my_vector::resize(size_t len) {
    if (this->is_small) {

    }
}

void my_vector::resize(size_t len, size_t val) {

}

void my_vector::correct() {
    if (!this->is_small && this->data.vals->size() == 1) {
        this->data.val = new *this->data.vals->[0];
        delete this->data.vals;
        this->is_small = true;
    }
}

void my_vector::make_own() {
    if (this->is_small) {
        if (this->data.val.use_count() > 1) {
            this->data.val = new (*this->data.val);
        }
    } else {
        if (this->data.vals.use_count() > 1) {
            std::vector<size_t>* curr = new std::vector<size_t>;
            for (size_t i = 0; i < this->data.vals->size(); ++i) {
                curr->push_back(this->data.vals->[i]);
            }
            this->data.vals = curr;
        }
    }
}

size_t& my_vector::operator[] (my_vector& v, const size_t index) {
    if (v.is_small) {
        if (v.data.val.use_count() > 1) {
            v.make_own();
        }
        return *v.data.val;
    } else {
        if (v.data.vals.use_count() > 1) {
            v.make_own();
        }
        return (*v.data.vals)[index];
    }
}