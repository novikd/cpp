#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <utility>
#include <gtest/gtest.h>
#include <iostream>
#include "big_integer.h"

namespace
{
    unsigned const number_of_iterations = 10;
    size_t const number_of_multipliers = 1000;

    int myrand()
    {
        int val = rand() - RAND_MAX / 2;
        if (val != 0)
            return val;
        else
            return 1;
    }
}

TEST(correctness, add_sub_randomized)
{
    for (unsigned itn = 0; itn != number_of_iterations; ++itn)
    {
        std::vector<int> multipliers;

        for (size_t i = 0; i != number_of_multipliers; ++i)
            multipliers.push_back(myrand());

        big_integer accumulator = 0;
        big_integer check;

        for (size_t i = 0; i != number_of_multipliers; ++i) {
            check = accumulator;
            accumulator += multipliers[i];
            if (accumulator - multipliers[i] != check) {
                std::cout<<"WTF\n";
            }
        }

        std::random_shuffle(multipliers.begin(), multipliers.end());

        for (size_t i = number_of_multipliers; i > 0; --i) {
            check = accumulator;
            accumulator -= multipliers[i - 1];
            if (accumulator + multipliers[i - 1] != check) {
                std::cout<<"Sad story\n";
            }
        }

        EXPECT_TRUE(accumulator == 0);
    }
}

namespace
{
    template <typename T>
    void erase_unordered(std::vector<T>& v, typename std::vector<T>::iterator pos)
    {
        std::swap(v.back(), *pos);
        v.pop_back();
    }

    template <typename T>
    T extract_random_element(std::vector<T>& v)
    {
        size_t index = rand() % v.size();
        T copy = v[index];
        erase_unordered(v, v.begin() + index);
        return copy;
    }

    template <typename T>
    void merge_two(std::vector<T>& v)
    {
        assert(v.size() >= 2);

        T a = extract_random_element(v);
        T b = extract_random_element(v);

        T ab = a + b;
        ASSERT_TRUE(ab - a == b);
        ASSERT_TRUE(ab - b == a);

        v.push_back(ab);
    }

    template <typename T>
    T merge_all(std::vector<T> v)
    {
        assert(!v.empty());

        while (v.size() >= 2) {
            merge_two(v);
        }

        return v[0];
    }
}

TEST(correctness, add_merge_randomized)
{
    for (unsigned itn = 0; itn != number_of_iterations; ++itn)
    {
        std::vector<big_integer> x;
        for (size_t i = 0; i != number_of_multipliers; ++i)
            x.push_back(myrand());

        big_integer a = merge_all(x);
        big_integer b = merge_all(x);

        EXPECT_TRUE(a == b);
    }
}