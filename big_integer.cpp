//
// Created by novik on 27.05.15.
//
#include "big_integer.h"
#include <vector>
#include <stdint.h>
#include <stddef.h>

#ifndef BIGINTEGER_BIG_INTEGER_H
#define BIGINTEGER_BIG_INTEGER_H
#endif //BIGINTEGER_BIG_INTEGER_H

const __int128_t base = static_cast<__int128_t>(1 << 64);

    std::vector<size_t> data;
    bool sign;

    big_integer::big_integer(int num) {
        if (num >= 0) {
            this->sign = false;
            this->data.push_back(num);
        } else {
            this->sign = true;
            this->data.push_back(-num);
        }
    }

    big_integer() :big_integer(0){}

    big_integer(bool signum, std::vector<size_t> val) {
        this->sign = signum;
        this->data = val;
    }

    big_integer(big_integer num) {
        this->sign = num.sign;
        this->data = num.data;
    }

    bool equals(big_integer num) {
        if (this->sign != num.sign || this->data.size() != num.data.size()) {
            return false;
        }
        for (size_t i = 0; i < num.data.size(); i++) {
            if (this->data[i] != num.data[i]) {
                return false;
            }
        }
        return true;
    }

    bool greater(big_integer num) {
        big_integer tmp = (*this).sub(num);
        if (tmp.sign) {
            return false;
        }
        if (tmp.equals(big_integer(0))) {
            return false;
        } else {
            return true;
        }
    }

    big_integer add(big_integer num) {
        if (this->sign && !num.sign) {
            this->sign = false;
            return num.sub(*this);
        }
        if (!this->sign && num.sign) {
            num.sign = false;
            return (*this).sub(num);
        }

        size_t carry = 0;
        for (size_t i = 0; i < this->data.size() || i < num.data.size(); i++) {
            __uint128_t curr = carry;
            if (i < this->data.size()) {
                curr += this->data[i];
            }
            if (i < num.data.size()) {
                curr += num.data[i];
            }
            if (i < this->data.size()) {
                this->data[i] = static_cast<size_t>(curr % base);
            } else {
                this->data.push_back(static_cast<size_t>(curr % base));
            }
            carry = static_cast<size_t>(curr / base);
        }

        if (carry != 0) {
            this->data.push_back(carry);
        }
        return *this;
    }

    big_integer sub(big_integer num) {
        if (this->sign && !num.sign) {
            num.sign = true;
            return (*this).add(num);
        }
        if (!this->sign && num.sign) {
            num.sign = false;
            return (*this).add(num);
        }

        if (num.data.size() > this->data.size() || (num.data.size() == this->data.size() && num.data.back() > this->data.back())) {
            void* ptr = &num;
            num = *this;
            this = static_cast<big_integer *>(ptr);
            this->sign = !this->sign;
        }

        __int128_t carry = 0;
        for (size_t i = 0; i < num.data.size() || carry != 0 ; i++) {
            carry = this->data[i] - carry - num.data[i];
            if (carry < 0) {
                this->data[i] = base + carry;
                carry = 1;
            } else {
                this->data[i] = carry;
                carry = 0;
            }
        }

        for (size_t i = this->data.size(); i > 1; i--) {
            if (this-data[i - 1] == 0) {
                this->data.pop_back();
            } else {
                break;
            }
        }

        return *this;
    }

    big_integer mul(big_integer num) {
        bool signum;
        if (this->sign != num.sign) {
            signum = true;
        } else {
            signum = false;
        }

        std::vector<size_t> val(this->data.size() + num.data.size(), 0);
        __int128_t carry = 0;
        for (size_t i = 0; i < num.data.size(); i++) {
            for (size_t j = 0; j < this->data.size() || carry > 0; j++) {
                __int128_t curr = val [i + j] + num.data[i] * (j < this->data.size() ? this->data[j] : 0) + carry;
                val[i + j] = static_cast<uint64_t>(curr % base);
                carry = curr / base;
            }
        }
        return big_integer(signum, val);
    }

    big_integer div(big_integer num) {
        if (this->sign != num.sign) {
            this->sign = true;
        } else {
            this->sign = false;
        }

    }

    big_integer code() {
        if (!this->sign) {
            return *this;
        }
        for (size_t i = 0; i < this->data.size(); i++) {
            this->data[i] = ~this->data[i];
        }
        size_t carry = 1;
        for (size_t i = 0; i < this->data.size(); i++) {
            __int128_t curr = this->data[i] + carry;
            this->data[i] = static_cast<size_t>(curr % base);
            carry = static_cast<size_t>(curr / base);
        }
        if (carry > 0) {
            this->data.push_back(carry);
        }
        return *this;
    }

    big_integer decode() {
        if (!this->sign) {
            return *this;
        }
        size_t carry = 1;
        for (size_t i = 0; i < this->data.size(); i++) {
            __int128_t curr = this->data[i] - carry;
            if (curr < 0) {
                this->data[i] = static_cast<size_t>(curr + base);
            } else {
                carry = 0;
                this->data[i] = static_cast<size_t>(curr);
            }
        }
        for (size_t i = 0; i < this->data.size(); i++) {
            this->data[i] = ~this->data[i];
        }
        return *this;
    }

    big_integer big_and(big_integer num) {
        if (this->data.size() < num.data.size()) {
            while (this->data.size() < num.data.size()) {
                this->data.push_back(0);
            }
        }
        if (this->data.size() > num.data.size()) {
            while (this->data.size() > num.data.size()) {
                num.data.push_back(0);
            }
        }
        big_integer tmp;
        if (this->sign) {
            tmp = (*this).code();
        } else {
            tmp = *this;
        }
        if (num.sign) {
            num = num.code();
        }
        std::vector<size_t> val;
        for (size_t i = 0; i < num.data.size(); i++) {
            val.push_back(num.data[i] & tmp.data[i]);
        }

        return big_integer(this->sign && num.sign, val);
    }

    big_integer big_or(big_integer num) {
        if (this->data.size() < num.data.size()) {
            while (this->data.size() < num.data.size()) {
                this->data.push_back(0);
            }
        }
        if (this->data.size() > num.data.size()) {
            while (this->data.size() > num.data.size()) {
                num.data.push_back(0);
            }
        }
        big_integer tmp;
        if (this->sign) {
            tmp = (*this).code();
        } else {
            tmp = *this;
        }
        if (num.sign) {
            num = num.code();
        }
        std::vector<size_t> val;
        for (size_t i = 0; i < num.data.size(); i++) {
            val.push_back(num.data[i] | tmp.data[i]);
        }

        return big_integer(this->sign || num.sign, val);
    }

    big_integer big_xor(big_integer num) {
        if (this->data.size() < num.data.size()) {
            while (this->data.size() < num.data.size()) {
                this->data.push_back(0);
            }
        }
        if (this->data.size() > num.data.size()) {
            while (this->data.size() > num.data.size()) {
                num.data.push_back(0);
            }
        }
        big_integer tmp;
        if (this->sign) {
            tmp = (*this).code();
        } else {
            tmp = *this;
        }
        if (num.sign) {
            num = num.code();
        }
        std::vector<size_t> val;
        for (size_t i = 0; i < num.data.size(); i++) {
            val.push_back(num.data[i] ^ tmp.data[i]);
        }

        return big_integer(this->sign ^ num.sign, val);
    }
