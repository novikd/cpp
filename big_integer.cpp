//
// Created by novik on 27.05.15.
//
#include "big_integer.h"
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <c++/iostream>

const __uint128_t base = static_cast<__uint128_t>(1) << 64;

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
    bool res = false;
    for (size_t i = 0; i < str.length() ; ++i) {
        if (i == 0 && str[i] == '-') {
            res = true;
            continue;
        }
        *this *= TEN;
        *this += big_integer(static_cast<int>(str[i] - '0'));
    }
    this->sign = res;
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
        return *this -= -num;
    }

    std::vector<size_t> val;
    size_t carry = 0;
    for (size_t i = 0; i < this->data.size() || i < num.data.size(); i++) {
        __uint128_t curr = carry;
        if (i < this->data.size()) {
            curr += static_cast<__uint128_t>(this->data[i]);
        }
        if (i < num.data.size()) {
            curr += static_cast<__uint128_t>(num.data[i]);
        }
        val.push_back(static_cast<size_t>(curr & (base - 1)));
        carry = static_cast<size_t>(curr >> 64);
    }

    if (carry != 0) {
        val.push_back(carry);
    }
    this->data = val;
    return (*this).correct();
}

big_integer& big_integer::operator-= (big_integer const &num) {
    if (this->sign ^ num.sign) {
        return *this += -num;
    }

    if (num.abs() > (*this).abs()) {
        *this = -(num - (*this));
        return *this;
    }

    __uint128_t carry = 0;
    for (size_t i = 0; i < num.data.size() || carry != 0 ; i++) {
        __int128_t curr = static_cast<__int128_t>(this->data[i]);
        curr -= carry;
        if (i < num.data.size()) {
            curr -= static_cast<__int128_t>(num.data[i]);
        }
        if (curr < 0) {
            curr += base;
            this->data[i] = static_cast<size_t>(curr);
            carry = 1;
        } else {
            this->data[i] = static_cast<size_t>(curr);
            carry = 0;
        }
    }

    return (*this).correct();
}

big_integer& big_integer::operator*= (big_integer const &num) {
    this->sign = this->sign ^ num.sign;

    __uint128_t carry = 0;
    size_t len = this->data.size();
    this->data.resize(len + num.data.size());
    for (size_t i = len; i > 0; --i) {
        __uint128_t mul = static_cast<__uint128_t>(this->data[i - 1]);
        this->data[i - 1] = 0;
        for (size_t j = 0; j < num.data.size() || carry > 0; ++j) {
            if (i + j - 1 < this->data.size()) {
                __uint128_t curr = this->data[i + j - 1];
                curr += mul * (j < num.data.size() ? static_cast<__uint128_t>(num.data[j]) : 0);
                curr += carry;
                this->data[i + j - 1] = static_cast<size_t>(curr);
                carry = curr >> 64;
            } else {
                this->data.push_back(static_cast<size_t>(carry));
                carry = 0;
            }
        }
    }

    return (*this).correct();
}

big_integer& big_integer::mul_long_short(size_t num) {
    size_t carry = 0;
    for (size_t i = 0; i < this->data.size(); ++i) {
        __uint128_t curr = static_cast<__uint128_t>(this->data[i]);
        curr *= static_cast<__uint128_t>(num);
        curr += static_cast<__uint128_t>(carry);
        this->data[i] = static_cast<size_t>(curr & (base - 1));
        carry = static_cast<size_t>(curr >> 64);
    }
    if (carry != 0) {
        this->data.push_back(carry);
    }
    return *this;
}

size_t div(size_t fs, size_t sc, size_t d) {
    __uint128_t qj = 0;
    qj = ((static_cast<__uint128_t>(fs) << 64) + static_cast<__uint128_t>(sc));
   
    qj /= static_cast<__uint128_t>(d);
   
    size_t result = static_cast<size_t>(base - 1);
    if (qj < (base - 1)) {
        result = static_cast<size_t>(qj);
    }

    return result;
}
 
big_integer& big_integer::operator/=(big_integer const& rhs) {
    bool sign = this->sign ^ rhs.sign;
    this->sign = false;
   
    big_integer right(rhs);
    right.sign = false;

    if (*this < right) {
        return *this = 0;
    }

    size_t norm_kf = static_cast<size_t>(base / (right.data.back() + 1));
    (*this).mul_long_short(norm_kf);
    right.mul_long_short(norm_kf);

    size_t n = right.data.size();
    size_t m = this->data.size() - right.data.size();
   
    big_integer q(0);
    q.data.resize(m + 1);
    big_integer shifted_right(right << (static_cast<int>(m) * 64));
   
    if (*this >= shifted_right) {
        q.data[m] = 1;
        *this -= shifted_right;
    }

    for (size_t j = m; j > 0; --j) {
        q.data[j - 1] = div(this->data[n + j - 1], this->data[n + j - 2], right.data[n - 1]);

        shifted_right >>= 64;
        big_integer curr(q.data[j - 1]);
        curr *= shifted_right;
        *this -= curr;

        size_t dec = 0;
        while (*this < 0) {
            dec += 1;
            *this += shifted_right;
        }
        q.data[j - 1] -= dec;
    }
   
    *this = q;
    this->sign  = sign;
    return (*this).correct();
}

big_integer& big_integer::operator%= (big_integer const &num) {
    big_integer tmp = (*this) / num;
    return *this -= (tmp * num);
}

big_integer& big_integer::correct() {
    for (size_t i = this->data.size(); i > 1; --i) {
        if (this->data.back() == 0) {
            this->data.pop_back();
        } else {
            break;
        }
    }
    return *this;
}

big_integer& big_integer::code() {
    if (!this->sign) {
        return *this;
    }
    for (size_t i = 0; i < this->data.size(); ++i) {
        this->data[i] = ~this->data[i];
    }
    __uint128_t carry = 1;
    for (size_t i = 0; i < this->data.size(); ++i) {
        __uint128_t curr = this->data[i];
        curr += carry;
        this->data[i] = static_cast<size_t>(curr % base);
        carry = curr / base;
    }
    return *this;
}

big_integer& big_integer::decode() {
    if (!this->sign) {
        return *this;
    }
    __int128_t carry = 1;
    for (size_t i = 0; i < this->data.size(); ++i) {
        __int128_t curr = this->data[i];
        curr -= carry;
        if (curr >= 0) {
            this->data[i] = static_cast<size_t>(curr);
            carry = 0;
        } else {
            curr += base;
            this->data[i] = static_cast<size_t>(curr);
            carry = 1;
        }
        this->data[i] = ~this->data[i];
    }
    return *this;
}

big_integer& big_integer::operator&=(big_integer const &num) {
    while (this->data.size() < num.data.size()) {
        this->data.push_back(0);
    }
    big_integer tmp = num;
    while (this->data.size() > tmp.data.size()) {
        tmp.data.push_back(0);
    }
    (*this).code();
    tmp.code();
    for (size_t i = 0; i < tmp.data.size(); i++) {
        this->data[i] &= tmp.data[i];
    }
    this->sign = this->sign && tmp.sign;
    (*this).decode();
    return (*this).correct();
}

big_integer& big_integer::operator|=(big_integer const &num) {
    while (this->data.size() < num.data.size()) {
        this->data.push_back(0);
    }
    big_integer tmp = num;
    while (this->data.size() > tmp.data.size()) {
        tmp.data.push_back(0);
    }
    (*this).code();
    tmp.code();
    for (size_t i = 0; i < tmp.data.size(); i++) {
        this->data[i] |= tmp.data[i];
    }
    this->sign = this->sign || tmp.sign;
    (*this).decode();
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
    (*this).code();
    tmp.code();
    for (size_t i = 0; i < tmp.data.size(); i++) {
        this->data[i] ^= tmp.data[i];
    }
    this->sign = this->sign ^ tmp.sign;
    (*this).decode();
    return (*this).correct();
}

big_integer& big_integer::operator<<=(int len) {
    size_t amount = static_cast<size_t>(len / 64);
    len %= 64;
    std::vector<size_t> val(amount, 0);
    __uint128_t carry = 0;
    for (size_t i = 0; i < this->data.size(); ++i) {
        __uint128_t curr = this->data[i];
        curr <<= len;
        curr += carry;
        val.push_back(static_cast<size_t>(curr % base));
        carry = curr >> 64;
    }
    if (carry != 0) {
        val.push_back(static_cast<size_t>(carry));
    }
    this->data = val;
    return (*this).correct();
}

big_integer& big_integer::operator>>=(int len) {
    size_t amount = static_cast<size_t>(len / 64);
    len %= 64;
    (*this).code();
    std::vector<size_t> val(this->data.size() - amount);
    size_t help = static_cast<size_t>(1 << len) - 1;
    size_t carry = 0;
    std::vector<size_t> current = this->data;
    for (size_t i = this->data.size(); i > amount; --i) {
        size_t curr = this->data[i - 1] & help;
        this->data[i - 1] >>= len;
        if (i == this->data.size()) {
            this->data[i - 1] += this->sign ? help << (64 - len) : 0;
        } else {
            this->data[i - 1] += carry;
        }
        carry = curr << (64 - len);
        val[i - amount - 1] = this->data[i - 1];
    }
    this->data = val;
    (*this).decode();
    return (*this).correct();
}

big_integer big_integer::operator+() const { return *this; }

big_integer big_integer::operator-() const {
    big_integer tmp = *this;
    tmp.sign = !tmp.sign;
    return tmp;
}

big_integer big_integer::operator~() const {
    big_integer tmp = *this;
    tmp.code();
    tmp.sign = !tmp.sign;
    for (size_t i = 0; i < tmp.data.size(); ++i) {
        tmp.data[i] = ~tmp.data[i];
    }
    return tmp.decode();
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
    if (frs.data.size() == snd.data.size() && frs.data.size() == 1 && frs.data[0] == snd.data[0] && frs.data[0] == 0) {
        return true;
    }
    if ((frs.sign ^ snd.sign) || (frs.data.size() != snd.data.size())) {
        return false;
    }
    for (size_t i = 0; i < frs.data.size(); ++i) {
        if (frs.data[i] != snd.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!= (big_integer const &frs, big_integer const &snd) { return !(frs == snd); }

bool operator< (big_integer const &frs, big_integer const &snd) {
    if (frs == snd) {
        return false;
    }
    if (frs.sign && !snd.sign) {
        return true;
    }
    if (!frs.sign && snd.sign) {
        return false;
    }
//compare abs
    bool small = true;
    if (frs.data.size() > snd.data.size()) {small = false;}
    if (frs.data.size() == snd.data.size()) {
        for (size_t i = frs.data.size(); i > 0; --i) {
            if (frs.data[i - 1] < snd.data[i - 1]) {
                break;
            }
            if (frs.data[i - 1] > snd.data[i - 1]) {
                small = false;
                break;
            }
        }
    }
    if (!(frs.sign ^ small)) {
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

big_integer operator+ (big_integer a, big_integer const &b) { return a += b; }

big_integer operator- (big_integer a, big_integer const &b) { return a -= b; }

big_integer operator* (big_integer a, big_integer const &b) { return a *= b;}

big_integer operator/ (big_integer a, big_integer const &b) { return a /= b;}

big_integer operator% (big_integer a, big_integer const &b) { return a %= b;}

big_integer operator& (big_integer a, big_integer const &b) { return a &= b; }

big_integer operator| (big_integer a, big_integer const &b) { return a |= b; }

big_integer operator^ (big_integer a, big_integer const &b) { return a ^= b; }

big_integer operator<< (big_integer a, int b) { return a <<= b; }

big_integer operator>> (big_integer a, int b) { return a >>= b; }

std::string to_string(big_integer const &a) {
    std::string result = "", sign = "";
    if (a == 0) {
        return "0";
    }
    big_integer num = a;
    const big_integer TEN(10);
    if (num.sign) {
        sign = "-";
        num.sign = false;
    }
    while (num > big_integer(0)) {
        big_integer curr = num % TEN;
        num /= TEN;
        result += static_cast<char>('0' + curr.data[0]);
    }
    std::reverse(result.begin(), result.end());
    return sign + result;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}

big_integer big_integer::abs() const {
    return this->sign ? -(*this) : *this;
}

big_integer::big_integer(size_t a)
    :big_integer(0)
{
    this->data[0] = a;
}
