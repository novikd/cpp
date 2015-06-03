//
// Created by novik on 27.05.15.
//
#include "big_integer.h"
#include <vector>
#include <stdint.h>

const __int128_t base = static_cast<__int128_t>(1 << 64);

std::vector<size_t> data;
bool sign;

big_integer::big_integer(int num) {
    if (num >= 0) {
        this->sign = false;
        this->data.push_back(static_cast<size_t>(num));
    } else {
        this->sign = true;
        this->data.push_back(static_cast<size_t>(-num));
    }
}

big_integer::big_integer() :big_integer(0){}


big_integer::big_integer(big_integer const &num) {
    this->sign = num.sign;
    this->data = num.data;
}

big_integer& big_integer::operator+=(big_integer const &num) {
    if (this->sign && !num.sign) {
        return num -= (-(*this));
    }
    if (!this->sign && num.sign) {
        return (*this) -= (-num);
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

big_integer& big_integer::operator-= (big_integer const &num) {
    if (this->sign && !num.sign) {
        return (*this) += (-num);
    }
    if (!this->sign && num.sign) {
        return (*this) += (-num);
    }

    if (num.data.size() > this->data.size() || (num.data.size() == this->data.size() && num.data.back() > this->data.back())) {
        return num -= (*this);
    }

    __int128_t carry = 0;
    for (size_t i = 0; i < num.data.size() || carry != 0 ; i++) {
        carry = this->data[i] - carry - num.data[i];
        if (carry < 0) {
            this->data[i] = static_cast<size_t>(base + carry);
            carry = 1;
        } else {
            this->data[i] = static_cast<size_t>(carry);
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

big_integer& big_integer::operator*= (big_integer const &num) {
    this->sign = this->sign ^ num.sign;

    __uint128_t carry = 0;
    size_t len = this->data.size();
    this->data.resize(len + num.data.size());
    for (size_t i = len; i > 0; --i) {
        bool stop = false;
        for (size_t j = num.data.size(); j > 0 || carry > 0; --j) {
            if (j == 0) {stop = true;}
            __int128_t curr = this->data[i + j - 2] + this->data[i - 1] * (!stop ? num.data[j] : 0) + carry;
            this->data[i + j] = static_cast<uint64_t>(curr % base);
            carry = static_cast<__uint128_t>(curr / base);
        }
    }
    return (*this);
}

big_integer& big_integer::operator/= (big_integer const &num) {
    if (this->sign != num.sign) {
        this->sign = true;
    } else {
        this->sign = false;
    }

}

big_integer& big_integer::operator&=(big_integer const &num) {
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

big_integer& big_integer::operator|=(big_integer const &num) {
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

big_integer& big_integer::operator^= (big_integer const &num) {
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
