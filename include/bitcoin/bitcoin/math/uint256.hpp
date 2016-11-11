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

/// A general purpose big number class with a subset of common operators.
//// Only operators for work computation are implemented (and identity).
class BC_API uint256_t
{
public:

    // Constructors.
    //-------------------------------------------------------------------------

    uint256_t();
    uint256_t(uint256_t&& other);
    uint256_t(const uint256_t& other);
    uint256_t(const hash_digest& hash);
    uint256_t(uint32_t value);

    // Properties.
    //-------------------------------------------------------------------------

    /// The minimum bit length of the number.
    size_t bit_length() const;

    /// The minimum byte length of the number.
    size_t byte_length() const;

    /// Get one of four 64 bit segments of the number.
    uint64_t operator[](size_t index) const;

    // Operators.
    //-------------------------------------------------------------------------

    //************************************************************************
    // CONSENSUS: uint256_t implements consensus critical overflow behavior
    // for all operators, specifically [~, -, *, +, ++, +=, *=, -=].
    //*************************************************************************

    bool operator>(const uint256_t& other) const;       // bc::chain
    bool operator<(const uint256_t& other) const;       // bc::chain
    bool operator>=(const uint256_t& other) const;      // bc::chain
    bool operator<=(const uint256_t& other) const;      // bc::chain
    bool operator==(const uint256_t& other) const;      // test
    bool operator!=(const uint256_t& other) const;      // test

    uint256_t operator~() const;                        // bc::chain
    uint256_t operator-() const;                        // internal
    uint256_t operator>>(uint32_t shift) const;         // bc::chain
    uint256_t operator+(const uint256_t& other) const;  // bc::chain
    uint256_t operator/(const uint256_t& other) const;  // bc::chain
    
    uint256_t& operator++();                            // internal
    uint256_t& operator=(uint32_t value);               // bc::blockchain
    uint256_t& operator>>=(uint32_t shift);             // internal
    uint256_t& operator<<=(uint32_t shift);             // internal
    uint256_t& operator*=(uint32_t value);              // bc::chain
    uint256_t& operator/=(uint32_t value);              // bc::chain
    uint256_t& operator+=(const uint256_t& other);      // bc::blockchain
    uint256_t& operator-=(const uint256_t& other);      // internal
    uint256_t& operator/=(const uint256_t& other);      // internal

protected:
    bool equals(uint32_t value) const;
    int compare(const uint256_t& other) const;

private:
    typedef std::array<uint32_t, 8> words;

    words words_;
};

} // namespace libbitcoin

#endif
