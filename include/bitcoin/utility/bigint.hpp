///*
// * Copyright 2013 William Swanson
// *
// * This program is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License, version 2, as
// * published by the Free Software Foundation.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// */
//#ifndef LIBBITCOIN_BIG_INT_HPP
//#define LIBBITCOIN_BIG_INT_HPP
//
//#include <bitcoin/utility/external/bigint.h>
//
//#include <vector>
//
//namespace libbitcoin {
//
//class bigint {
//public:
//    ~bigint()
//    {
//        bigint_free(bi_);
//    }
//    bigint()
//    {
//        bi_ = bigint_nan();
//    }
//    bigint(bigint_t orphan)
//    {
//        bi_ = orphan;
//    }
//    bigint(const bigint& copy)
//    {
//        bi_ = bigint_copy(copy.bi_);
//    }
//    bigint(bigint&& move)
//    {
//        bi_ = move.bi_;
//        move.bi_ = bigint_nan();
//    }
//    bigint(int i)
//    {
//        bi_ = bigint_from_int(i);
//    }
//    //TODO: Add constructors for binary data
//
//    // Assignment:
//    bigint& operator=(const bigint& copy)
//    {
//        if (this != &copy)
//        {
//            bigint_free(bi_);
//            bi_ = bigint_copy(copy.bi_);
//        }
//        return *this;
//    }
//    bigint& operator=(bigint&& move)
//    {
//        if (this != &move)
//        {
//            bigint_free(bi_);
//            bi_ = move.bi_;
//            move.bi_ = bigint_nan();
//        }
//        return *this;
//    }
//
//    // Conversion:
//    int to_int()
//    {
//        return bigint_to_int(bi_);
//    }
//    // TODO: Add conversions to binary data
//
//    // TODO: Add string conversions
//
//    // Information:
//    bool is_nan()
//    {
//        return bigint_is_nan(bi_);
//    }
//    size_t size()
//    {
//        return bigint_size(bi_);
//    }
//    uint8_t sign()
//    {
//        return bigint_sign(bi_);
//    }
//    operator bool()
//    {
//        return !bigint_is_zero(bi_);
//    }
//
//    // Comparison:
//    bool operator==(const bigint& rhs) { return bigint_compare(bi_, rhs.bi_) == 0; }
//    bool operator!=(const bigint& rhs) { return bigint_compare(bi_, rhs.bi_) != 0; }
//    bool operator< (const bigint& rhs) { return bigint_compare(bi_, rhs.bi_) <  0; }
//    bool operator<=(const bigint& rhs) { return bigint_compare(bi_, rhs.bi_) <= 0; }
//    bool operator> (const bigint& rhs) { return bigint_compare(bi_, rhs.bi_) >  0; }
//    bool operator>=(const bigint& rhs) { return bigint_compare(bi_, rhs.bi_) >= 0; }
//
//    // Shifter:
//    bigint operator>>(int shift)
//    {
//        return bigint(bigint_shift(bi_, -shift));
//    }
//    bigint operator<<(int shift)
//    {
//        return bigint(bigint_shift(bi_, shift));
//    }
//
//    // Bitwise:
//    bigint operator~()
//    {
//        return bigint(bigint_not(bi_));
//    }
//    bigint operator&(const bigint& rhs)
//    {
//        return bigint(bigint_and(bi_, rhs.bi_));
//    }
//    bigint operator|(const bigint& rhs)
//    {
//        return bigint(bigint_or(bi_, rhs.bi_));
//    }
//    bigint operator^(const bigint& rhs)
//    {
//        return bigint(bigint_xor(bi_, rhs.bi_));
//    }
//
//    // ALU:
//    bigint operator-()
//    {
//        return bigint(bigint_neg(bi_));
//    }
//    bigint operator+(const bigint& rhs)
//    {
//        return bigint(bigint_add(bi_, rhs.bi_));
//    }
//    bigint operator-(const bigint& rhs)
//    {
//        return bigint(bigint_sub(bi_, rhs.bi_));
//    }
//    bigint operator*(const bigint& rhs)
//    {
//        return bigint(bigint_mul(bi_, rhs.bi_));
//    }
//
//    // Divider:
//    bigint operator/(const bigint& rhs)
//    {
//        bigint_div_t d = bigint_div(bi_, rhs.bi_);
//        bigint_free(d.r);
//        return bigint(d.q);
//    }
//    bigint operator%(const bigint& rhs)
//    {
//        bigint_div_t d = bigint_div(bi_, rhs.bi_);
//        bigint_free(d.q);
//        return bigint(d.r);
//    }
//
//private:
//    bigint_t bi_;
//};
//
//bigint& operator>>=(bigint& lhs, int shift) { return lhs = lhs >> shift; }
//bigint& operator<<=(bigint& lhs, int shift) { return lhs = lhs << shift; }
//bigint& operator&=(bigint& lhs, const bigint& rhs) { return lhs = lhs & rhs; }
//bigint& operator|=(bigint& lhs, const bigint& rhs) { return lhs = lhs | rhs; }
//bigint& operator^=(bigint& lhs, const bigint& rhs) { return lhs = lhs ^ rhs; }
//bigint& operator+=(bigint& lhs, const bigint& rhs) { return lhs = lhs + rhs; }
//bigint& operator-=(bigint& lhs, const bigint& rhs) { return lhs = lhs - rhs; }
//bigint& operator*=(bigint& lhs, const bigint& rhs) { return lhs = lhs * rhs; }
//bigint& operator/=(bigint& lhs, const bigint& rhs) { return lhs = lhs / rhs; }
//bigint& operator%=(bigint& lhs, const bigint& rhs) { return lhs = lhs % rhs; }
//
//}
//
//#endif