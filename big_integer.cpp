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
            curr += this->data[i];
        }
        if (i < num.data.size()) {
            curr += num.data[i];
        }
        val.push_back(static_cast<size_t>(curr % base));
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

    if ((num.sign && num < *this) || (!num.sign && num > *this)) {
        *this = -(num - (*this));
        return *this;
    }

    __int128_t carry = 0;
    for (size_t i = 0; i < num.data.size() || carry != 0 ; i++) {
        carry = this->data[i] - carry - (i < num.data.size() ? num.data[i] : 0);
        if (carry < 0) {
            this->data[i] = static_cast<size_t>(base + carry);
            carry = 1;
        } else {
            this->data[i] = static_cast<size_t>(carry);
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
        __uint128_t mul = this->data[i - 1];
        this->data[i - 1] = 0;
        for (size_t j = 0; j < num.data.size() || carry > 0; ++j) {
            if (i + j - 1 < this->data.size()) {
                __uint128_t curr = this->data[i + j - 1] + mul * (j < num.data.size() ? num.data[j] : 0) + carry;
                this->data[i + j - 1] = static_cast<size_t>(curr - ((curr >> 64) << 64));
                carry = curr >> 64;
            } else {
                this->data.push_back(static_cast<size_t>(carry));
                carry = 0;
            }
        }
    }

    return (*this).correct();
}

big_integer& big_integer::operator/= (big_integer const &num) {
    bool res = this->sign ^ num.sign;
    this->sign = false;
    big_integer tmp = num;
    tmp.sign = false;

    if (this->data.size() < tmp.data.size()) {
        *this = big_integer(0);
        return *this;
    }
    //Making numbers normolized
    size_t help = static_cast<size_t>(base / (num.data.back() + 1));
    big_integer comp;
    comp.data[0] = help;
    tmp *= comp;
    *this *= comp;

    big_integer quotient;
    size_t dif = this->data.size() - tmp.data.size();
    size_t n = tmp.data.size();
    quotient.data.resize(dif + 1, 0);
    comp = big_integer(1) << (dif * 64);
    comp *= tmp;
    if (*this >= comp) {
        quotient.data[dif] = 1;
        *this -= comp;
    }
    for (size_t i = dif; i > 0; --i) {
        __uint128_t curr = this->data[n + i - 1];
        curr <<= 64;
        curr += this->data[n + i - 2];
        curr /= tmp.data[n - 1];
        if (base - 1 < curr) {
            curr = base - 1;
        }
        big_integer temp = 0;
        temp.data[0] = static_cast<size_t>(curr);
        comp = tmp << ((i - 1) * 64);
        *this -= comp * temp;

        //std::cout << *this;
        while (*this < 0) {
            curr -= 1;
            *this += comp;
            //std::cout << static_cast<size_t>(curr) << "\n";
        }
        quotient.data[i - 1] = static_cast<size_t>(curr);
    }
    quotient.sign = res;
    *this = quotient;
    //std::cout << "Divided\n";
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

    bool small = true;
    if (frs.data.size() > snd.data.size()) {small = false;}
    if (frs.data.size() == snd.data.size()) {
        for (size_t i = frs.data.size(); i > 0 && small; --i) {
            if (frs.data[i - 1] < snd.data[i - 1]) {
                break;
            }
            if (frs.data[i - 1] > snd.data[i - 1]) {
                small = false;
                break;
            }
        }
    }
    if ((!frs.sign && !small) || (snd.sign && small)) {
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
    if (a == big_integer(0)) {
        return "0";
    }
    big_integer num = a;
    const big_integer TEN = big_integer(10);
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