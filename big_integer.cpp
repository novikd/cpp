//
// Created by novik on 27.05.15.
//
#include "big_integer.h"
#include <vector>
#include <stdint.h>

const __int128_t base = static_cast<__int128_t>(1 << 64);

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

big_integer::~big_integer() { }

big_integer& big_integer::operator= (big_integer const &num) {
    this->data = num.data;
    this->sign = num.sign;
    return *this;
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
        return (*this) += -(num);
    }
    if (!this->sign && num.sign) {
        return (*this) += -(num);
    }

    if (num.data.size() > this->data.size() || (num.data.size() == this->data.size() && num.data.back() > this->data.back())) {
        big_integer tmp = -(num -= (*this));
        return tmp;
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
            __int128_t curr = this->data[i + j - 2] + this->data[i - 1] * (!stop ? num.data[j - 1] : 0) + carry;
            this->data[i + j - 2] = static_cast<uint64_t>(curr % base);
            carry = static_cast<__uint128_t>(curr / base);
        }
    }
    return (*this);
}

big_integer& big_integer::operator/= (big_integer const &num) {
    this->sign = this->sign ^ num.sign;

    if (this->data.size() < num.data.size()) {
        big_integer tmp = big_integer(0);
        return tmp;
    }

    size_t dif = this->data.size() - num.data.size();
    std::vector<size_t> val(dif);
}

const big_integer& code(big_integer const &num) {
    if (!num.sign) {
        return num;
    }
    std::vector<size_t> val;
    for (size_t i = 0; i < num.data.size(); ++i) {
        val.push_back(~num.data[i]);
    }
    __uint128_t carry = 1;
    for (size_t i = 0; i < val.size(); ++i) {
        __uint128_t curr = num.data[i];
        curr += carry;
        val[i] = static_cast<size_t>(curr % base);
        carry = curr / base;
    }
    big_integer tmp;
    tmp.sign = carry != 0 ? !num.sign : num.sign;
    tmp.data = val;
    return tmp;
}

const big_integer& decode(big_integer const &num) {
    if (!num.sign) {
        return num;
    }
    std::vector<size_t> val(num.data.size());
    __int128_t carry = 1;
    for (size_t i = 0; i < val.size(); ++i) {
        __int128_t curr = num.data[i];
        curr -= carry;
        if (curr >= 0) {
            val[i] = static_cast<size_t>(curr);
            carry = 0;
        } else {
            curr += base;
            val[i] = static_cast<size_t>(curr);
            carry = 1;
        }
        val[i] = ~val[i];
    }
    big_integer tmp;
    tmp.sign = carry != 0 ? !num.sign : num.sign;
    tmp.data = val;
    return tmp;
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
    big_integer tmp1 = code(*this), tmp2 = code(num);
    for (size_t i = 0; i < tmp1.data.size(); i++) {
        tmp1.data[i] &= tmp2.data[i];
    }
    tmp1.sign = tmp1.sign && tmp2.sign;
    *this = decode(tmp1);
    return *this;
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
    big_integer tmp1 = code(*this), tmp2 = code(num);
    for (size_t i = 0; i < tmp1.data.size(); i++) {
        tmp1.data[i] |= tmp2.data[i];
    }
    tmp1.sign = tmp1.sign || tmp2.sign;
    *this = decode(tmp1);
    return *this;
}

big_integer& big_integer::operator^=(big_integer const &num) {
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
    big_integer tmp1 = code(*this), tmp2 = code(num);
    for (size_t i = 0; i < tmp1.data.size(); i++) {
        tmp1.data[i] ^= tmp2.data[i];
    }
    tmp1.sign = tmp1.sign ^ tmp2.sign;
    *this = decode(tmp1);
    return *this;
}

big_integer& big_integer::operator<<=(int len) {
    size_t amount = static_cast<size_t>(len / 64);
    len %= 64;
    std::vector<size_t> val(amount, 0);
    __uint128_t carry = 0;
    for (size_t i = 0; i < this->data.size(); ++i) {
        __uint128_t curr = this->data[i];
        curr <<= len;
        val.push_back(static_cast<size_t>((curr +carry) % base));
        carry = curr / base;
    }
    if (carry != 0) {
        val.push_back(static_cast<size_t>(carry));
    }
    this->data = val;
    return *this;
}

big_integer& big_integer::operator>>=(int len) {
    size_t amount = static_cast<size_t>(len / 64);
    len %= 64;
    std::vector<size_t> val(this->data.size() - amount);
    __uint128_t carry = 0;
    for (size_t i = this->data.size(); i > amount; --i) {
        __uint128_t curr = this->data[i - 1];
        curr >>= len;
        val[i - amount - 1] = static_cast<size_t>((curr +carry) % base);
        carry = curr / base;
    }
    this->data = val;
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer tmp = *this;
    tmp.sign = !tmp.sign;
    return tmp;
}

big_integer big_integer::operator~() const {
    big_integer tmp = code(*this);
    tmp.sign = !tmp.sign;
    for (size_t i = 0; i < tmp.data.size(); ++i) {
        tmp.data[i] = ~tmp.data[i];
    }
    return decode(tmp);
}

big_integer& big_integer::operator++() {
    *this += big_integer(1);
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer tmp = *this;
    *this += big_integer(1);
    return tmp;
}

big_integer& big_integer::operator--() {
    *this -= big_integer(1);
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer tmp = *this;
    *this -= big_integer(1);
    return tmp;
}