//
// Created by novik on 27.05.15.
//
#include "big_integer.h"
#include <vector>
#include <stdint.h>
#include <algorithm>

__uint128_t base = static_cast<__uint128_t>(1 << 64);

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

big_integer::big_integer(std::string const& str)
    :big_integer(0)
{
    big_integer TEN = big_integer(10);
    for (size_t i = str.length(); i > 1; --i) {
        if (i == 1 && str[i - 1] == '-') {
            this->sign = true;
        }
        *this *= TEN;
        *this += big_integer(static_cast<int>(str[i - 1] - '0'));
    }
}

big_integer::~big_integer() { }

big_integer& big_integer::operator= (big_integer const &num) {
    this->data = num.data;
    this->sign = num.sign;
    return *this;
}

big_integer& big_integer::operator+=(big_integer const &num) {
    if (this->sign && !num.sign) {
        *this = num - (-(*this));
        return *this;
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
    if (this->sign ^ !num.sign) {
        return (*this) += -(num);
    }

    if (num.data.size() > this->data.size() || (num.data.size() == this->data.size() && num.data.back() > this->data.back())) {
        *this = -(num - (*this));
        return *this;
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
            __uint128_t curr = this->data[i + j - 2] + this->data[i - 1] * (!stop ? num.data[j - 1] : 0) + carry;
            this->data[i + j - 2] = static_cast<size_t>(curr % base);
            carry = curr / base;
        }
    }
    return (*this);
}

big_integer& big_integer::operator/= (big_integer const &num) {
    if (this->data.size() < num.data.size()) {
        *this = big_integer(0);
        return *this;
    }
    big_integer tmp = num;
    while (tmp.data.back() < (base >> 1)) {
        tmp <<= 1;
        *this <<= 1;
    }
    big_integer quotient;
    size_t dif = this->data.size() - tmp.data.size();
    size_t n = tmp.data.size();
    quotient.data.resize(dif + 1);

    if (*this >= (tmp << dif)) {
        quotient.data[dif] = 1;
        *this -= (tmp << dif);
    } else {
        quotient.data[dif] = 0;
    }

    for (size_t i = dif; dif > 0; ++dif) {
        __uint128_t curr = this->data[n + i - 1];
        curr += this->data[n + i - 2];
        curr /= tmp.data[n - 1];
        if (base - 1 < curr) {
            curr = base - 1;
        }
        *this -= (big_integer(curr) << i) * tmp;
        while (*this < big_integer(0)) {
            curr -= 1;
            *this += (tmp << i);
        }
        quotient.data[i] = static_cast<size_t>(curr);
    }
    quotient.sign = this->sign ^ num.sign;
    *this = quotient;
    return *this;
}

big_integer& big_integer::operator%= (big_integer const &num) {
    if (this->data.size() < num.data.size()) {
        *this = big_integer(0);
        return *this;
    }
    big_integer tmp = num;
    while (tmp.data.back() < (base >> 1)) {
        tmp <<= 1;
        *this <<= 1;
    }
    size_t dif = this->data.size() - tmp.data.size();
    size_t n = tmp.data.size();

    for (size_t i = dif; dif > 0; ++dif) {
        __uint128_t curr = this->data[n + i - 1];
        curr += this->data[n + i - 2];
        curr /= tmp.data[n - 1];
        if (base - 1 < curr) {
            curr = base - 1;
        }
        *this -= (big_integer(curr) << i) * tmp;
        while (*this < big_integer(0)) {
            curr -= 1;
            *this += (tmp << i);
        }
    }
    return *this;
}

big_integer& code(big_integer const &num) {
    big_integer tmp = num;
    if (tmp >= big_integer(0)) {
        return tmp;
    }
    tmp = ~tmp;
    tmp += big_integer(1);
    return tmp;
}

big_integer& decode(big_integer const &num) {
    big_integer tmp = num;
    if (tmp >= big_integer(0)) {
        return tmp;
    }
    tmp -= big_integer(1);
    tmp = ~tmp;
    return tmp;
}

big_integer& big_integer::operator&=(big_integer const &num) {
    while (this->data.size() < num.data.size()) {
        this->data.push_back(0);
    }
    big_integer tmp = num;
    while (this->data.size() > tmp.data.size()) {
        tmp.data.push_back(0);
    }
    big_integer tmp1 = code(*this), tmp2 = code(tmp);
    for (size_t i = 0; i < tmp1.data.size(); i++) {
        tmp1.data[i] &= tmp2.data[i];
    }
    tmp1.sign = tmp1.sign && tmp2.sign;
    *this = decode(tmp1);
    return *this;
}

big_integer& big_integer::operator|=(big_integer const &num) {
    while (this->data.size() < num.data.size()) {
        this->data.push_back(0);
    }
    big_integer tmp = num;
    while (this->data.size() > tmp.data.size()) {
        tmp.data.push_back(0);
    }
    big_integer tmp1 = code(*this), tmp2 = code(tmp);
    for (size_t i = 0; i < tmp1.data.size(); i++) {
        tmp1.data[i] |= tmp2.data[i];
    }
    tmp1.sign = tmp1.sign || tmp2.sign;
    *this = decode(tmp1);
    return *this;
}

big_integer& big_integer::operator^=(big_integer const &num) {
    while (this->data.size() < num.data.size()) {
        this->data.push_back(0);
    }
    big_integer tmp = num;
    while (this->data.size() > tmp.data.size()) {
        tmp.data.push_back(0);
    }
    big_integer tmp1 = code(*this), tmp2 = code(tmp);
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

big_integer big_integer::operator+() const { return *this; }

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

bool operator== (big_integer const &frs, big_integer const &snd) {
    big_integer tmp1 = frs, tmp2 = snd;
    for (size_t i = tmp1.data.size(); i > 1; --i) {
        if (tmp1.data[i - 1] == 0) {
            tmp1.data.pop_back();
        } else {
            break;
        }
    }
    for (size_t i = tmp2.data.size(); i > 1; --i) {
        if (tmp2.data[i - 1] == 0) {
            tmp2.data.pop_back();
        } else {
            break;
        }
    }
    if (tmp1.sign != tmp2.sign || tmp1.data.size() != tmp2.data.size()) {
        return false;
    }
    for (size_t i = 0; i < tmp1.data.size(); ++i) {
        if (tmp1.data[i] != tmp2.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!= (big_integer const &frs, big_integer const &snd) { return !(frs == snd); }

bool operator< (big_integer const &frs, big_integer const &snd) {
    big_integer tmp1 = frs, tmp2 = snd;
    for (size_t i = tmp1.data.size(); i > 1; --i) {
        if (tmp1.data[i - 1] == 0) {
            tmp1.data.pop_back();
        } else {
            break;
        }
    }
    for (size_t i = tmp2.data.size(); i > 1; --i) {
        if (tmp2.data[i - 1] == 0) {
            tmp2.data.pop_back();
        } else {
            break;
        }
    }
    if (tmp1.sign && !tmp2.sign) {
        return true;
    }
    if (!tmp1.sign && tmp2.sign) {
        return false;
    }

    bool small = true;
    if (tmp1.data.size() >= tmp2.data.size()) {small = false;}
    for (size_t i = tmp1.data.size(); i > 1 && small; --i) {
        if (tmp1.data[i - 1] >= tmp2.data[i - 1]) {
            small = false;
        }
    }

    if ((!tmp1.sign && !small) || (tmp1.sign && small)) {
        return false;
    }
    return true;
}

bool operator>(big_integer const &a, big_integer const &b) { return b < a; }

bool operator<=(big_integer const &a, big_integer const &b) {
    return (a < b) || (a == b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return (a > b) || (a == b);
}

big_integer operator+ (big_integer &a, big_integer const &b) { return a += b; }

big_integer operator- (big_integer &a, big_integer const &b) { return a -= b; }

big_integer operator* (big_integer &a, big_integer const &b) { return a *= b;}

big_integer operator/ (big_integer &a, big_integer const &b) { return a /= b;}

big_integer operator% (big_integer &a, big_integer const &b) { return a %= b;}

big_integer operator& (big_integer &a, big_integer const &b) { return a &= b; }

big_integer operator| (big_integer &a, big_integer const &b) { return a |= b; }

big_integer operator^ (big_integer &a, big_integer const &b) { return a ^= b; }

big_integer operator<< (big_integer &a, int b) { return a <<= b; }

big_integer operator>> (big_integer &a, int b) { return a >>= b; }

std::string to_string(big_integer const &a) {
    std::string result = "";
    if (a == big_integer(0)) {
        return "0";
    }
    big_integer num = a;
    const big_integer TEN = big_integer(10);
    while (num > big_integer(0)) {
        big_integer curr = num;
        curr %= TEN;
        num /= TEN;
        result += static_cast<char>('0' + curr.data[0]);
    }
    if (num.sign) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}
