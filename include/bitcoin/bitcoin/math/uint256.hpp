// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef LIBBBITCOIN_UINT256_HPP
#define LIBBBITCOIN_UINT256_HPP

#include <cstdint>
#include <cstring>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

class BC_API compact_number
{
public:
    compact_number(uint32_t value);
    uint32_t value() const;

private:
    const uint32_t value_;
};

class BC_API uint256_t
{
public:

    // Constructors.
    //-------------------------------------------------------------------------

    uint256_t();
    uint256_t(uint256_t&& other);
    uint256_t(const uint256_t& other);
    uint256_t(hash_digest&& hash);
    uint256_t(const hash_digest& hash);
    uint256_t(const compact_number& compact);
    uint256_t(uint32_t value);

    // Properties.
    //-------------------------------------------------------------------------

    bool overflow() const;
    uint32_t bits() const;
    compact_number to_compact() const;

    // Operators.
    //-----------------------------------------------------------------------------

    bool operator>(const uint256_t& other) const;
    bool operator<(const uint256_t& other) const;
    bool operator>=(const uint256_t& other) const;
    bool operator<=(const uint256_t& other) const;
    bool operator==(const uint256_t& other) const;

    const uint256_t operator~() const;
    const uint256_t operator-() const;
    const uint256_t operator>>(int shift) const;
    const uint256_t operator+(const uint256_t& other) const;
    const uint256_t operator/(const uint256_t& other) const;

    uint256_t& operator++();
    uint256_t& operator<<=(int32_t shift);
    uint256_t& operator>>=(int32_t shift);
    uint256_t& operator=(uint32_t value);
    uint256_t& operator*=(uint32_t value);
    uint256_t& operator/=(uint32_t value);
    uint256_t& operator+=(const uint256_t& other);
    uint256_t& operator-=(const uint256_t& other);
    uint256_t& operator/=(const uint256_t& other);

protected:
    static bool from_compact(uint256_t& out, const compact_number& compact);

    bool equals(uint32_t value) const;
    int compare(const uint256_t& other) const;

protected:
    static const uint32_t full_bits_ = 256;
    static const uint32_t word_bits_ = sizeof(uint32_t) * 8;
    static const uint32_t width_ = full_bits_ / word_bits_;

    const bool overflow_;
    uint32_t buffer_[width_];
};

} // namespace libbitcoin

#endif
