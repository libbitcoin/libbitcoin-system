/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "test.hpp"

// Guard type assumptions within the codebase.

// size_t/signed_size_t
// ----------------------------------------------------------------------------

// This are design limitations, and not a matter of C++ specification.
static_assert(sizeof(char) == one);
static_assert(
    is_same_size<size_t, uint32_t> ||
    is_same_size<size_t, uint64_t>);

// signed_size_t

// This tests that signed_size_t conforms to the design limitation.
static_assert(
    is_same_size<signed_size_t, int32_t> ||
    is_same_size<signed_size_t, int64_t>);

// Depends on construction of signed_size_t using std::make_signed_t<size_t>,
// as this is how the to_signed_type/to_unsigned_type conversions also work.
static_assert(
    is_same_type<to_signed_type<size_t>, signed_size_t>&&
    is_same_type<to_unsigned_type<signed_size_t>, size_t>);

// This is implied by above.
static_assert(is_same_size<size_t, signed_size_t>);

// signed_size_t may not be the same type as either, despite having same size.
// For example, long and int may both be 32 bit types, but not the same type.
////static_assert(
////    is_same_type<signed_size_t, int32_t> ||
////    is_same_type<signed_size_t, int64_t>);
#ifdef HAVE_MSC
static_assert(is_same_size<long, int>); // not assured on all platforms.
#endif
static_assert(!is_same_type<long, int>); // but always different types.

// signed_type/unsigned_type
// ----------------------------------------------------------------------------

static_assert(is_same_type<signed_type<>, signed_size_t>);
static_assert(is_same_type<signed_type<0>, signed_size_t>);
static_assert(is_same_type<signed_type<1>, int8_t>);
static_assert(is_same_type<signed_type<2>, int16_t>);
static_assert(is_same_type<signed_type<3>, int32_t>);
static_assert(is_same_type<signed_type<4>, int32_t>);
static_assert(is_same_type<signed_type<5>, int64_t>);
static_assert(is_same_type<signed_type<6>, int64_t>);
static_assert(is_same_type<signed_type<7>, int64_t>);
static_assert(is_same_type<signed_type<8>, int64_t>);

static_assert(is_same_type<unsigned_type<>, size_t>);
static_assert(is_same_type<unsigned_type<0>, size_t>);
static_assert(is_same_type<unsigned_type<1>, uint8_t>);
static_assert(is_same_type<unsigned_type<2>, uint16_t>);
static_assert(is_same_type<unsigned_type<3>, uint32_t>);
static_assert(is_same_type<unsigned_type<4>, uint32_t>);
static_assert(is_same_type<unsigned_type<5>, uint64_t>);
static_assert(is_same_type<unsigned_type<6>, uint64_t>);
static_assert(is_same_type<unsigned_type<7>, uint64_t>);
static_assert(is_same_type<unsigned_type<8>, uint64_t>);

// to_[]_type
// ----------------------------------------------------------------------------

static_assert(is_same_type<to_signed_type<uint8_t>, int8_t>);
static_assert(is_same_type<to_signed_type<uint16_t>, int16_t>);
static_assert(is_same_type<to_signed_type<uint32_t>, int32_t>);
static_assert(is_same_type<to_signed_type<uint64_t>, int64_t>);
static_assert(is_same_type<to_signed_type<int8_t>, int8_t>);
static_assert(is_same_type<to_signed_type<int16_t>, int16_t>);
static_assert(is_same_type<to_signed_type<int32_t>, int32_t>);
static_assert(is_same_type<to_signed_type<int64_t>, int64_t>);
////static_assert(is_same_type<to_signed_type<unsigned char>, signed char>);
////static_assert(is_same_type<to_signed_type<unsigned short>, signed short>);
////static_assert(is_same_type<to_signed_type<unsigned int>, signed int>);
////static_assert(is_same_type<to_signed_type<unsigned long>, signed long>);
////static_assert(is_same_type<to_signed_type<unsigned long long>, signed long long>);

static_assert(is_same_type<to_unsigned_type<int8_t>, uint8_t>);
static_assert(is_same_type<to_unsigned_type<int16_t>, uint16_t>);
static_assert(is_same_type<to_unsigned_type<int32_t>, uint32_t>);
static_assert(is_same_type<to_unsigned_type<int64_t>, uint64_t>);
static_assert(is_same_type<to_unsigned_type<uint8_t>, uint8_t>);
static_assert(is_same_type<to_unsigned_type<uint16_t>, uint16_t>);
static_assert(is_same_type<to_unsigned_type<uint32_t>, uint32_t>);
static_assert(is_same_type<to_unsigned_type<uint64_t>, uint64_t>);
static_assert(is_same_type<to_unsigned_type<size_t>, size_t>);
////static_assert(is_same_type<to_signed_type<signed char>, unsigned char>);
////static_assert(is_same_type<to_signed_type<signed short>, unsigned short>);
////static_assert(is_same_type<to_signed_type<signed int>, unsigned int>);
////static_assert(is_same_type<to_signed_type<signed long>, unsigned long>);
////static_assert(is_same_type<to_signed_type<signed long long>, unsigned long long>);

static_assert(is_same_type<to_size_type<uint8_t>, size_t>);
static_assert(is_same_type<to_size_type<uint16_t>, size_t>);
static_assert(is_same_type<to_size_type<uint32_t>, size_t>);
static_assert(is_same_type<to_size_type<uint64_t>, size_t>);
static_assert(is_same_type<to_size_type<unsigned char>, size_t>);
static_assert(is_same_type<to_size_type<int8_t>, signed_size_t>);
static_assert(is_same_type<to_size_type<int16_t>, signed_size_t>);
static_assert(is_same_type<to_size_type<int32_t>, signed_size_t>);
static_assert(is_same_type<to_size_type<int64_t>, signed_size_t>);
static_assert(is_same_type<to_size_type<signed char>, signed_size_t>);

static_assert(is_same_type<to_common_type<bool, int>, int>);
static_assert(is_same_type<to_common_type<int, bool>, int>);
static_assert(is_same_type<to_common_type<uint8_t, int>, int>);
static_assert(is_same_type<to_common_type<uint16_t, int>, int>);
static_assert(is_same_type<to_common_type<uint32_t, int>, uint32_t>);
static_assert(is_same_type<to_common_type<int, uint32_t>, uint32_t>);
static_assert(is_same_type<to_common_type<uint32_t, int>, uint32_t>);
static_assert(is_same_type<to_common_type<uint32_t, uint64_t>, uint64_t>);
static_assert(is_same_type<to_common_type<uint32_t, int64_t>, int64_t>);
static_assert(is_same_type<to_common_type<uint32_t, uint64_t>, uint64_t>);

static_assert(is_same_type<to_common_sized_type<bool>, int>);
static_assert(is_same_type<to_common_sized_type<int8_t>, int>);
static_assert(is_same_type<to_common_sized_type<int16_t>, int>);
static_assert(is_same_type<to_common_sized_type<int32_t>, int32_t>);
static_assert(is_same_type<to_common_sized_type<int64_t>, int64_t>);
static_assert(is_same_type<to_common_sized_type<uint8_t>, int>);
static_assert(is_same_type<to_common_sized_type<uint16_t>, int>);
static_assert(is_same_type<to_common_sized_type<uint32_t>, uint32_t>);
static_assert(is_same_type<to_common_sized_type<uint64_t>, uint64_t>);

static_assert(is_same_type<to_greater_type<uint8_t, int64_t>, int64_t>);
static_assert(is_same_type<to_greater_type<uint8_t, int16_t>, int16_t>);
static_assert(is_same_type<to_greater_type<uint8_t, int32_t>, int32_t>);
static_assert(is_same_type<to_greater_type<uint8_t, int64_t>, int64_t>);
static_assert(is_same_type<to_greater_type<uint8_t, uint8_t>, uint8_t>);
static_assert(is_same_type<to_greater_type<uint16_t, uint8_t>, uint16_t>);
static_assert(is_same_type<to_greater_type<uint32_t, uint8_t>, uint32_t>);

// Must prefer left over right in the case of same size.
static_assert(is_same_type<to_greater_type<int8_t, uint8_t>, int8_t>);
static_assert(is_same_type<to_greater_type<uint8_t, int8_t>, uint8_t>);
static_assert(is_same_type<to_greater_type<int16_t, uint16_t>, int16_t>);
static_assert(is_same_type<to_greater_type<uint16_t, int16_t>, uint16_t>);
static_assert(is_same_type<to_greater_type<int32_t, uint32_t>, int32_t>);
static_assert(is_same_type<to_greater_type<uint32_t, int32_t>, uint32_t>);
static_assert(is_same_type<to_greater_type<int64_t, uint64_t>, int64_t>);
static_assert(is_same_type<to_greater_type<uint64_t, int64_t>, uint64_t>);
static_assert(is_same_type<to_greater_type<size_t, signed_size_t>, size_t>);
static_assert(is_same_type<to_greater_type<signed_size_t, size_t>, signed_size_t>);

static_assert(is_same_type<to_promoted_type<int>, int64_t>);
static_assert(is_same_type<to_promoted_type<unsigned int>, uint64_t>);
static_assert(is_same_type<to_promoted_type<int8_t>, int64_t>);
static_assert(is_same_type<to_promoted_type<int16_t>, int64_t>);
static_assert(is_same_type<to_promoted_type<int32_t>, int64_t>);
static_assert(is_same_type<to_promoted_type<int64_t>, int64_t>);
static_assert(is_same_type<to_promoted_type<signed_size_t>, int64_t>);
static_assert(is_same_type<to_promoted_type<uint8_t>, uint64_t>);
static_assert(is_same_type<to_promoted_type<uint16_t>, uint64_t>);
static_assert(is_same_type<to_promoted_type<uint32_t>, uint64_t>);
static_assert(is_same_type<to_promoted_type<uint64_t>, uint64_t>);
static_assert(is_same_type<to_promoted_type<size_t>, uint64_t>);
static_assert(is_same_type<to_promoted_type<uintx>, uintx>);
static_assert(is_same_type<to_promoted_type<uint128_t>, uint128_t>);
static_assert(is_same_type<to_promoted_type<uint256_t>, uint256_t>);

// uintx_t<>
// ----------------------------------------------------------------------------

static_assert(is_same_type<uintx_t<5u>, uint5_t>);
static_assert(is_same_type<uintx_t<11u>, uint11_t>);
static_assert(is_same_type<uintx_t<48u>, uint48_t>);
static_assert(is_same_type<uintx_t<128u>, uint128_t>);
static_assert(is_same_type<uintx_t<160u>, uint160_t>);
static_assert(is_same_type<uintx_t<256u>, uint256_t>);
static_assert(is_same_type<uintx_t<512u>, uint512_t>);

static_assert(is_same_type<uintx,
    boost::multiprecision::cpp_int>);
static_assert(is_same_type<uintx_t<1u>,
    boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<1u, 1u,
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>>>);

static_assert(is_same_type<unsigned_exact_type<0>, size_t>);
static_assert(is_same_type<unsigned_exact_type<1>, uint8_t>);
static_assert(is_same_type<unsigned_exact_type<2>, uint16_t>);
static_assert(is_same_type<unsigned_exact_type<3>, uintx_t<to_bits(3u)>>);
static_assert(is_same_type<unsigned_exact_type<4>, uint32_t>);
static_assert(is_same_type<unsigned_exact_type<5>, uintx_t<to_bits(5u)>>);
static_assert(is_same_type<unsigned_exact_type<6>, uintx_t<to_bits(6u)>>);
static_assert(is_same_type<unsigned_exact_type<7>, uintx_t<to_bits(7u)>>);
static_assert(is_same_type<unsigned_exact_type<8>, uint64_t>);
static_assert(is_same_type<unsigned_exact_type<9>, uintx_t<to_bits(9u)>>);
static_assert(is_same_type<unsigned_exact_type<10>, uintx_t<to_bits(10u)>>);
static_assert(is_same_type<unsigned_exact_type<32>, uintx_t<to_bits(32u)>>);
static_assert(is_same_type<unsigned_exact_type<256>, uintx_t<to_bits(256u)>>);
static_assert(is_same_type<unsigned_exact_type<512>, uintx_t<to_bits(512u)>>);
static_assert(is_same_type<unsigned_exact_type<1024>, uintx_t<to_bits(1024u)>>);
static_assert(is_same_type<unsigned_exact_type<2048>, uintx_t<to_bits(2048u)>>);
