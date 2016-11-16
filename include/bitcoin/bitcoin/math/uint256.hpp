/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBBITCOIN_UINT256_HPP
#define LIBBBITCOIN_UINT256_HPP

#include <boost/multiprecision/cpp_int.hpp>

namespace libbitcoin {

/// libbitcoin namespace alias for boost big integer type.
typedef boost::multiprecision::uint256_t uint256_t;

////#include <iostream>
////#include <bitcoin/bitcoin/define.hpp>
////
/////// A general purpose big number class with a subset of common operators.
/////// Only operators for work computation are implemented (and identity).
////class BC_API uint256_t
////{
////public:
////
////    // Constructors.
////    //-------------------------------------------------------------------------
////
////    uint256_t();
////    uint256_t(uint256_t&& other);
////    uint256_t(const uint256_t& other);
////    uint256_t(const hash_digest& hash);
////    uint256_t(uint32_t value);
////
////    // Properties.
////    //-------------------------------------------------------------------------
////
////    /// The minimum bit length of the number.
////    size_t bit_length() const;
////
////    /// The minimum byte length of the number.
////    size_t byte_length() const;
////
////    /// The 256 bit hash corresponding to the value.
////    hash_digest hash() const;
////
////    /// Get one of four 64 bit segments of the number.
////    uint64_t operator[](size_t index) const;
////
////    // Operators.
////    //-------------------------------------------------------------------------
////
////    //************************************************************************
////    // CONSENSUS: uint256_t implements consensus critical overflow behavior
////    // for all operators, specifically [~, -, *, +, ++, +=, *=, -=].
////    //*************************************************************************
////
////    bool operator>(const uint256_t& other) const;       // bc::chain
////    bool operator<(const uint256_t& other) const;       // bc::chain
////    bool operator>=(const uint256_t& other) const;      // bc::chain
////    bool operator<=(const uint256_t& other) const;      // bc::chain
////    bool operator==(const uint256_t& other) const;      // test
////    bool operator!=(const uint256_t& other) const;      // test
////
////    uint256_t operator~() const;                        // bc::chain
////    uint256_t operator-() const;                        // internal
////    uint256_t operator>>(uint32_t shift) const;         // bc::chain
////    uint256_t operator+(const uint256_t& other) const;  // bc::chain
////    uint256_t operator/(const uint256_t& other) const;  // bc::chain
////    
////    uint256_t& operator++();                            // internal
////    uint256_t& operator=(uint32_t value);               // bc::blockchain
////    uint256_t& operator>>=(uint32_t shift);             // internal
////    uint256_t& operator<<=(uint32_t shift);             // internal
////    uint256_t& operator*=(uint32_t value);              // bc::chain
////    uint256_t& operator/=(uint32_t value);              // bc::chain
////    uint256_t& operator+=(const uint256_t& other);      // bc::blockchain
////    uint256_t& operator-=(const uint256_t& other);      // internal
////    uint256_t& operator/=(const uint256_t& other);      // internal
////
////    // Serialization.
////    //-------------------------------------------------------------------------
////
////    friend std::istream& operator>>(std::istream& input, uint256_t& argument);
////    friend std::ostream& operator<<(std::ostream& output, 
////        const uint256_t& argument);
////
////protected:
////    bool equals(uint32_t value) const;
////    int compare(const uint256_t& other) const;
////
////private:
////    typedef std::array<uint32_t, 8> words;
////
////    // We use a 32 bit words member so we can take advantage of integral
////    // addition and multiplication while capturing the overflow.
////    words words_;
////};

} // namespace libbitcoin

#endif
