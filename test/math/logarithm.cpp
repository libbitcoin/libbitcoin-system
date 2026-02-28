/**
* Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"

// is_power2

////static_assert(!is_power2(-1));
static_assert(!is_power2(0u));
static_assert(!is_power2(3u));
static_assert(!is_power2(5u));
static_assert(!is_power2(6u));
static_assert(!is_power2(7u));
static_assert(!is_power2(9u));
static_assert(!is_power2(10u));
static_assert(!is_power2(11u));
static_assert(!is_power2(12u));
static_assert(!is_power2(sub1(9223372036854775808u)));
static_assert(!is_power2(add1(9223372036854775808u)));

static_assert( is_power2(1u));
static_assert( is_power2(2u));
static_assert( is_power2(4u));
static_assert( is_power2(8u));
static_assert( is_power2(16u));
static_assert( is_power2(32u));
static_assert( is_power2(64u));
static_assert( is_power2(128u));
static_assert( is_power2(256u));
static_assert( is_power2(512u));
static_assert( is_power2(1024u));
static_assert( is_power2(2048u));
static_assert( is_power2(4096u));
static_assert( is_power2(8192u));
static_assert( is_power2(16384u));
static_assert( is_power2(32768u));
static_assert( is_power2(65536u));
static_assert( is_power2(131072u));
static_assert( is_power2(262144u));
static_assert( is_power2(524288u));
static_assert( is_power2(1048576u));
static_assert( is_power2(2097152u));
static_assert( is_power2(4194304u));
static_assert( is_power2(8388608u));
static_assert( is_power2(16777216u));
static_assert( is_power2(33554432u));
static_assert( is_power2(67108864u));
static_assert( is_power2(134217728u));
static_assert( is_power2(268435456u));
static_assert( is_power2(536870912u));
static_assert( is_power2(1073741824u));
static_assert( is_power2(2147483648u));
static_assert( is_power2(4294967296u));
static_assert( is_power2(8589934592u));
static_assert( is_power2(17179869184u));
static_assert( is_power2(34359738368u));
static_assert( is_power2(68719476736u));
static_assert( is_power2(137438953472u));
static_assert( is_power2(274877906944u));
static_assert( is_power2(549755813888u));
static_assert( is_power2(1099511627776u));
static_assert( is_power2(2199023255552u));
static_assert( is_power2(4398046511104u));
static_assert( is_power2(8796093022208u));
static_assert( is_power2(17592186044416u));
static_assert( is_power2(35184372088832u));
static_assert( is_power2(70368744177664u));
static_assert( is_power2(140737488355328u));
static_assert( is_power2(281474976710656u));
static_assert( is_power2(562949953421312u));
static_assert( is_power2(1125899906842624u));
static_assert( is_power2(2251799813685248u));
static_assert( is_power2(4503599627370496u));
static_assert( is_power2(9007199254740992u));
static_assert( is_power2(18014398509481984u));
static_assert( is_power2(36028797018963968u));
static_assert( is_power2(72057594037927936u));
static_assert( is_power2(144115188075855872u));
static_assert( is_power2(288230376151711744u));
static_assert( is_power2(576460752303423488u));
static_assert( is_power2(1152921504606846976u));
static_assert( is_power2(2305843009213693952u));
static_assert( is_power2(4611686018427387904u));
static_assert( is_power2(9223372036854775808u));

// ceilinged_log

static_assert(ceilinged_log(-2, 1u) == 0u);
static_assert(ceilinged_log(2u, -1) == 0u);
static_assert(ceilinged_log(-2, -1) == 0u);

static_assert(ceilinged_log(-0, 0) == 0u);
static_assert(ceilinged_log(-0, 0u) == 0u);
static_assert(ceilinged_log(0, 0) == 0u);
static_assert(ceilinged_log(0, 0u) == 0u);

static_assert(ceilinged_log(1, 0) == 0u);
static_assert(ceilinged_log(1, 0u) == 0u);
static_assert(ceilinged_log(1u, 0) == 0u);
static_assert(ceilinged_log(1u, 0u) == 0u);

static_assert(ceilinged_log(-0, 1) == 0u);
static_assert(ceilinged_log(-0, 1u) == 0u);
static_assert(ceilinged_log(0, 1) == 0u);
static_assert(ceilinged_log(0, 1u) == 0u);

static_assert(ceilinged_log(1, 1) == 0u);
static_assert(ceilinged_log(1, 1u) == 0u);
static_assert(ceilinged_log(1u, 1) == 0u);
static_assert(ceilinged_log(1u, 1u) == 0u);

static_assert(ceilinged_log(2, 1) == 0u);
static_assert(ceilinged_log(2, 1u) == 0u);
static_assert(ceilinged_log(2u, 1) == 0u);
static_assert(ceilinged_log(2u, 1u) == 0u);

static_assert(ceilinged_log(2, 2) == 1u);
static_assert(ceilinged_log(2, 2u) == 1u);
static_assert(ceilinged_log(2u, 2) == 1u);
static_assert(ceilinged_log(2u, 2u) == 1u);

static_assert(ceilinged_log(3, 3) == 1u);
static_assert(ceilinged_log(3, 3u) == 1u);
static_assert(ceilinged_log(3u, 3) == 1u);
static_assert(ceilinged_log(3u, 3u) == 1u);

static_assert(ceilinged_log(2, 4) == 2u);
static_assert(ceilinged_log(2, 4u) == 2u);
static_assert(ceilinged_log(2u, 4) == 2u);
static_assert(ceilinged_log(2u, 4u) == 2u);

static_assert(ceilinged_log<int>(10, 1) == 0);
static_assert(ceilinged_log<int>(10, 10) == 1);
static_assert(ceilinged_log<int>(10, 100) == 2);
static_assert(ceilinged_log<int>(10, 1000) == 3);
static_assert(ceilinged_log<int>(10, 10000) == 4);
static_assert(ceilinged_log<int>(10, 100000) == 5);
static_assert(ceilinged_log<int>(10, 1000000) == 6);

// floored_log

static_assert(floored_log(-2, 2u) == 0u);
static_assert(floored_log(2u, -2) == 0u);
static_assert(floored_log(-2, -2) == 0u);

static_assert(floored_log(-0, 0) == 0u);
static_assert(floored_log(-0, 0u) == 0u);
static_assert(floored_log(0, 0) == 0u);
static_assert(floored_log(0, 0u) == 0u);

static_assert(floored_log(1, 0) == 0u);
static_assert(floored_log(1, 0u) == 0u);
static_assert(floored_log(1u, 0) == 0u);
static_assert(floored_log(1u, 0u) == 0u);

static_assert(floored_log(-0, 1) == 0u);
static_assert(floored_log(-0, 1u) == 0u);
static_assert(floored_log(0, 1) == 0u);
static_assert(floored_log(0, 1u) == 0u);

static_assert(floored_log(1, 1) == 0u);
static_assert(floored_log(1, 1u) == 0u);
static_assert(floored_log(1u, 1) == 0u);
static_assert(floored_log(1u, 1u) == 0u);

static_assert(floored_log(2, 1) == 0u);
static_assert(floored_log(2, 1u) == 0u);
static_assert(floored_log(2u, 1) == 0u);
static_assert(floored_log(2u, 1u) == 0u);

static_assert(floored_log(2, 2) == 1u);
static_assert(floored_log(2, 2u) == 1u);
static_assert(floored_log(2u, 2) == 1u);
static_assert(floored_log(2u, 2u) == 1u);

static_assert(floored_log(3, 2) == 0u);
static_assert(floored_log(3, 2u) == 0u);
static_assert(floored_log(3u, 2) == 0u);
static_assert(floored_log(3u, 2u) == 0u);

static_assert(floored_log(2, 3) == 1u);
static_assert(floored_log(2, 3u) == 1u);
static_assert(floored_log(2u, 3) == 1u);
static_assert(floored_log(2u, 3u) == 1u);

static_assert(floored_log<int>(10, 1) == 0);
static_assert(floored_log<int>(10, 10) == 1);
static_assert(floored_log<int>(10, 100) == 2);
static_assert(floored_log<int>(10, 1000) == 3);
static_assert(floored_log<int>(10, 10000) == 4);
static_assert(floored_log<int>(10, 100000) == 5);
static_assert(floored_log<int>(10, 1000000) == 6);

// ceilinged_log2

static_assert(ceilinged_log2(0) == 0u);
static_assert(ceilinged_log2(0u) == 0u);
static_assert(ceilinged_log2(-42) == 0u);
static_assert(ceilinged_log2(max_uint8) == to_bits(sizeof(uint8_t)));
static_assert(ceilinged_log2(max_uint16) == to_bits(sizeof(uint16_t)));
static_assert(ceilinged_log2(max_uint32) == to_bits(sizeof(uint32_t)));

// Also mixing in signed and unsigned types.

static_assert(ceilinged_log2((1u << 0) + 0) == 0u);
static_assert(ceilinged_log2((1u << 0) + 1) == 1u);
static_assert(ceilinged_log2((1u << 1) + 0) == 1u);
static_assert(ceilinged_log2((1u << 1) + 1) == 2u);
static_assert(ceilinged_log2((1u << 2) + 0) == 2u);
static_assert(ceilinged_log2((1u << 2) + 1) == 3u);
static_assert(ceilinged_log2((1u << 3) + 0) == 3u);
static_assert(ceilinged_log2((1u << 3) + 1) == 4u);
static_assert(ceilinged_log2((1u << 4) + 0) == 4u);
static_assert(ceilinged_log2((1u << 4) + 1) == 5u);
static_assert(ceilinged_log2((1u << 5) + 0) == 5u);
static_assert(ceilinged_log2((1u << 5) + 1) == 6u);
static_assert(ceilinged_log2((1u << 6) + 0) == 6u);
static_assert(ceilinged_log2((1u << 6) + 1) == 7u);
static_assert(ceilinged_log2((1u << 7) + 0) == 7u);
static_assert(ceilinged_log2((1u << 7) + 1) == 8u);
static_assert(ceilinged_log2((1u << 8) + 0) == 8u);
static_assert(ceilinged_log2((1u << 8) + 1) == 9u);
static_assert(ceilinged_log2((1u << 9) + 0) == 9u);
static_assert(ceilinged_log2((1u << 9) + 1) == 10u);
static_assert(ceilinged_log2<int>((1 << 10) + 0) == 10);
static_assert(ceilinged_log2<int>((1 << 10) + 1) == 11);
static_assert(ceilinged_log2<int>((1 << 11) + 0) == 11);
static_assert(ceilinged_log2<int>((1 << 11) + 1) == 12);
static_assert(ceilinged_log2<int>((1 << 12) + 0) == 12);
static_assert(ceilinged_log2<int>((1 << 12) + 1) == 13);
static_assert(ceilinged_log2<int>((1 << 13) + 0) == 13);
static_assert(ceilinged_log2<int>((1 << 13) + 1) == 14);
static_assert(ceilinged_log2<int>((1 << 14) + 0) == 14);
static_assert(ceilinged_log2<int>((1 << 14) + 1) == 15);
static_assert(ceilinged_log2<int>((1 << 15) + 0) == 15);
static_assert(ceilinged_log2<int>((1 << 15) + 1) == 16);
static_assert(ceilinged_log2<int>((1 << 16) + 0) == 16);
static_assert(ceilinged_log2<int>((1 << 16) + 1) == 17);
static_assert(ceilinged_log2<int>((1 << 17) + 0) == 17);
static_assert(ceilinged_log2<int>((1 << 17) + 1) == 18);
static_assert(ceilinged_log2<int>((1 << 18) + 0) == 18);
static_assert(ceilinged_log2<int>((1 << 18) + 1) == 19);
static_assert(ceilinged_log2<int>((1 << 19) + 0) == 19);
static_assert(ceilinged_log2<int>((1 << 19) + 1) == 20);
static_assert(ceilinged_log2<int>((1 << 20) + 0) == 20);
static_assert(ceilinged_log2<int>((1 << 20) + 1) == 21);
static_assert(ceilinged_log2<int>((1 << 21) + 0) == 21);
static_assert(ceilinged_log2<int>((1 << 21) + 1) == 22);
static_assert(ceilinged_log2<int>((1 << 22) + 0) == 22);
static_assert(ceilinged_log2<int>((1 << 22) + 1) == 23);
static_assert(ceilinged_log2<int>((1 << 23) + 0) == 23);
static_assert(ceilinged_log2<int>((1 << 23) + 1) == 24);
static_assert(ceilinged_log2<int>((1 << 24) + 0) == 24);
static_assert(ceilinged_log2<int>((1 << 24) + 1) == 25);
static_assert(ceilinged_log2<int>((1 << 25) + 0) == 25);
static_assert(ceilinged_log2<int>((1 << 25) + 1) == 26);
static_assert(ceilinged_log2<int>((1 << 26) + 0) == 26);
static_assert(ceilinged_log2<int>((1 << 26) + 1) == 27);
static_assert(ceilinged_log2<int>((1 << 27) + 0) == 27);
static_assert(ceilinged_log2<int>((1 << 27) + 1) == 28);
static_assert(ceilinged_log2<int>((1 << 28) + 0) == 28);
static_assert(ceilinged_log2<int>((1 << 28) + 1) == 29);
static_assert(ceilinged_log2<int>((1 << 29) + 0) == 29);
static_assert(ceilinged_log2((1 << 29) + 1) == 30u);
static_assert(ceilinged_log2((1 << 30) + 0) == 30u);
static_assert(ceilinged_log2((1 << 30) + 1) == 31u);
static_assert(ceilinged_log2((1ll << 31) + 0) == 31u);
static_assert(ceilinged_log2((1ll << 31) + 1) == 32u);

static_assert(ceilinged_log2<int>(power2(15u)) == 15);
static_assert(ceilinged_log2<unsigned>(power2(15u)) == 15u);
static_assert(ceilinged_log2<int>(power(2, 15u)) == 15);
static_assert(ceilinged_log2<unsigned>(power(2u, 15u)) == 15u);

// Also mixing in signed and unsigned types.

static_assert(ceilinged_log2(uint256_t((1u << 0) + 0)) == 0u);
static_assert(ceilinged_log2(uint256_t((1u << 0) + 1)) == 1u);
static_assert(ceilinged_log2(uint256_t((1u << 1) + 0)) == 1u);
static_assert(ceilinged_log2(uint256_t((1u << 1) + 1)) == 2u);
static_assert(ceilinged_log2(uint256_t((1u << 2) + 0)) == 2u);
static_assert(ceilinged_log2(uint256_t((1u << 2) + 1)) == 3u);
static_assert(ceilinged_log2(uint256_t((1u << 3) + 0)) == 3u);
static_assert(ceilinged_log2(uint256_t((1u << 3) + 1)) == 4u);
static_assert(ceilinged_log2(uint256_t((1u << 4) + 0)) == 4u);
static_assert(ceilinged_log2(uint256_t((1u << 4) + 1)) == 5u);
static_assert(ceilinged_log2(uint256_t((1u << 5) + 0)) == 5u);
static_assert(ceilinged_log2(uint256_t((1u << 5) + 1)) == 6u);
static_assert(ceilinged_log2(uint256_t((1u << 6) + 0)) == 6u);
static_assert(ceilinged_log2(uint256_t((1u << 6) + 1)) == 7u);
static_assert(ceilinged_log2(uint256_t((1u << 7) + 0)) == 7u);
static_assert(ceilinged_log2(uint256_t((1u << 7) + 1)) == 8u);
static_assert(ceilinged_log2(uint256_t((1u << 8) + 0)) == 8u);
static_assert(ceilinged_log2(uint256_t((1u << 8) + 1)) == 9u);
static_assert(ceilinged_log2(uint256_t((1u << 9) + 0)) == 9u);
static_assert(ceilinged_log2(uint256_t((1u << 9) + 1)) == 10u);
static_assert(ceilinged_log2<int>(uint256_t((1 << 10) + 0)) == 10);
static_assert(ceilinged_log2<int>(uint256_t((1 << 10) + 1)) == 11);
static_assert(ceilinged_log2<int>(uint256_t((1 << 11) + 0)) == 11);
static_assert(ceilinged_log2<int>(uint256_t((1 << 11) + 1)) == 12);
static_assert(ceilinged_log2<int>(uint256_t((1 << 12) + 0)) == 12);
static_assert(ceilinged_log2<int>(uint256_t((1 << 12) + 1)) == 13);
static_assert(ceilinged_log2<int>(uint256_t((1 << 13) + 0)) == 13);
static_assert(ceilinged_log2<int>(uint256_t((1 << 13) + 1)) == 14);
static_assert(ceilinged_log2<int>(uint256_t((1 << 14) + 0)) == 14);
static_assert(ceilinged_log2<int>(uint256_t((1 << 14) + 1)) == 15);
static_assert(ceilinged_log2<int>(uint256_t((1 << 15) + 0)) == 15);
static_assert(ceilinged_log2<int>(uint256_t((1 << 15) + 1)) == 16);
static_assert(ceilinged_log2<int>(uint256_t((1 << 16) + 0)) == 16);
static_assert(ceilinged_log2<int>(uint256_t((1 << 16) + 1)) == 17);
static_assert(ceilinged_log2<int>(uint256_t((1 << 17) + 0)) == 17);
static_assert(ceilinged_log2<int>(uint256_t((1 << 17) + 1)) == 18);
static_assert(ceilinged_log2<int>(uint256_t((1 << 18) + 0)) == 18);
static_assert(ceilinged_log2<int>(uint256_t((1 << 18) + 1)) == 19);
static_assert(ceilinged_log2<int>(uint256_t((1 << 19) + 0)) == 19);
static_assert(ceilinged_log2<int>(uint256_t((1 << 19) + 1)) == 20);
static_assert(ceilinged_log2<int>(uint256_t((1 << 20) + 0)) == 20);
static_assert(ceilinged_log2<int>(uint256_t((1 << 20) + 1)) == 21);
static_assert(ceilinged_log2<int>(uint256_t((1 << 21) + 0)) == 21);
static_assert(ceilinged_log2<int>(uint256_t((1 << 21) + 1)) == 22);
static_assert(ceilinged_log2<int>(uint256_t((1 << 22) + 0)) == 22);
static_assert(ceilinged_log2<int>(uint256_t((1 << 22) + 1)) == 23);
static_assert(ceilinged_log2<int>(uint256_t((1 << 23) + 0)) == 23);
static_assert(ceilinged_log2<int>(uint256_t((1 << 23) + 1)) == 24);
static_assert(ceilinged_log2<int>(uint256_t((1 << 24) + 0)) == 24);
static_assert(ceilinged_log2<int>(uint256_t((1 << 24) + 1)) == 25);
static_assert(ceilinged_log2<int>(uint256_t((1 << 25) + 0)) == 25);
static_assert(ceilinged_log2<int>(uint256_t((1 << 25) + 1)) == 26);
static_assert(ceilinged_log2<int>(uint256_t((1 << 26) + 0)) == 26);
static_assert(ceilinged_log2<int>(uint256_t((1 << 26) + 1)) == 27);
static_assert(ceilinged_log2<int>(uint256_t((1 << 27) + 0)) == 27);
static_assert(ceilinged_log2<int>(uint256_t((1 << 27) + 1)) == 28);
static_assert(ceilinged_log2<int>(uint256_t((1 << 28) + 0)) == 28);
static_assert(ceilinged_log2<int>(uint256_t((1 << 28) + 1)) == 29);
static_assert(ceilinged_log2<int>(uint256_t((1 << 29) + 0)) == 29);
static_assert(ceilinged_log2(uint256_t((1 << 29) + 1)) == 30u);
static_assert(ceilinged_log2(uint256_t((1 << 30) + 0)) == 30u);
static_assert(ceilinged_log2(uint256_t((1 << 30) + 1)) == 31u);
static_assert(ceilinged_log2(uint256_t((1ll << 31) + 0)) == 31u);
static_assert(ceilinged_log2(uint256_t((1ll << 31) + 1)) == 32u);
static_assert(ceilinged_log2(to_uintx(base16_hash("ffffffffffffffff"))) == 64u);
static_assert(ceilinged_log2(to_uintx(base16_hash("010000000000000000"))) == 64u);
static_assert(ceilinged_log2(to_uintx(base16_hash("ffffffffffffffffffffffffffffffff"))) == 128u);
static_assert(ceilinged_log2(to_uintx(base16_hash("0100000000000000000000000000000000"))) == 128u);
static_assert(ceilinged_log2(to_uintx(base16_hash("f000000000000000000000000000000000000000000000000000000000000000"))) == 256u);
static_assert(ceilinged_log2(to_uintx(base16_hash("010000000000000000000000000000000000000000000000000000000000000000"))) == 256u);
static_assert(ceilinged_log2(to_uintx(base16_hash("f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))) == 512u);
static_assert(ceilinged_log2(to_uintx(base16_hash("0100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))) == 512u);

// ceilinged_log256
static_assert(ceilinged_log256(0x0100000000000000ull) == 7);
static_assert(ceilinged_log256(0x01000000000000ull) == 6);
static_assert(ceilinged_log256(0x010000000000ull) == 5);
static_assert(ceilinged_log256(0x0100000000ull) == 4);
static_assert(ceilinged_log256(0x01000000ull) == 3);
static_assert(ceilinged_log256(0x010000ull) == 2);
static_assert(ceilinged_log256(0x0100ull) == 1);
static_assert(ceilinged_log256(0x01ull) == 0);
static_assert(ceilinged_log256(0x00ull) == 0);

// uint256_t
static_assert(ceilinged_log256(uint256_t(0x0100000000000000ull)) == 7);
static_assert(ceilinged_log256(uint256_t(0x01000000000000ull)) == 6);
static_assert(ceilinged_log256(uint256_t(0x010000000000ull)) == 5);
static_assert(ceilinged_log256(uint256_t(0x0100000000ull)) == 4);
static_assert(ceilinged_log256(uint256_t(0x01000000ull)) == 3);
static_assert(ceilinged_log256(uint256_t(0x010000ull)) == 2);
static_assert(ceilinged_log256(uint256_t(0x0100ull)) == 1);
static_assert(ceilinged_log256(uint256_t(0x01ull)) == 0);
static_assert(ceilinged_log256(uint256_t(0x00ull)) == 0);

// ceilinged_log<base>
static_assert(ceilinged_log<0u>(42) == 0u);
static_assert(ceilinged_log<2u>((1u << 0) + 0) == 0u);
static_assert(ceilinged_log<2u>((1u << 0) + 1) == 1u);
static_assert(ceilinged_log<2u>((1u << 1) + 0) == 1u);
static_assert(ceilinged_log<2u>((1u << 1) + 1) == 2u);
static_assert(ceilinged_log<2u>((1u << 2) + 0) == 2u);
static_assert(ceilinged_log<2u>((1u << 2) + 1) == 3u);
static_assert(ceilinged_log<2u>((1u << 3) + 0) == 3u);
static_assert(ceilinged_log<2u>((1u << 3) + 1) == 4u);
static_assert(ceilinged_log<2u>((1u << 4) + 0) == 4u);
static_assert(ceilinged_log<2u>((1u << 4) + 1) == 5u);
static_assert(ceilinged_log<2u>((1u << 5) + 0) == 5u);
static_assert(ceilinged_log<2u>((1u << 5) + 1) == 6u);
static_assert(ceilinged_log<2u>((1u << 6) + 0) == 6u);
static_assert(ceilinged_log<2u>((1u << 6) + 1) == 7u);
static_assert(ceilinged_log<2u>((1u << 7) + 0) == 7u);
static_assert(ceilinged_log<2u>((1u << 7) + 1) == 8u);
static_assert(ceilinged_log<2u>((1u << 8) + 0) == 8u);
static_assert(ceilinged_log<2u>((1u << 8) + 1) == 9u);
static_assert(ceilinged_log<2u>((1u << 9) + 0) == 9u);
static_assert(ceilinged_log<2u, size_t>((1u << 9) + 1) == 10u);
static_assert(ceilinged_log<256u>(0x0100000000000001ull) == 8);
static_assert(ceilinged_log<256u>(0x0100000000000000ull) == 7);
static_assert(ceilinged_log<256u>(0x01000000000000ull) == 6);
static_assert(ceilinged_log<256u>(0x010000000000ull) == 5);
static_assert(ceilinged_log<256u>(0x0100000000ull) == 4);
static_assert(ceilinged_log<256u>(0x01000000ull) == 3);
static_assert(ceilinged_log<256u>(0x010000ull) == 2);
static_assert(ceilinged_log<256u>(0x0100ull) == 1);
static_assert(ceilinged_log<256u>(0x01ull) == 0);
static_assert(ceilinged_log<256u>(0x00ull) == 0);
static_assert(ceilinged_log<10u, int>(1000000) == 6);
static_assert(ceilinged_log<10u>(1000000) == 6);
static_assert(ceilinged_log<2>(uint256_t{ 255 }) == 8u);

// floored_log2

static_assert(floored_log2(0) == 0u);
static_assert(floored_log2(0u) == 0u);
static_assert(floored_log2(-42) == 0u);
static_assert(floored_log2(max_uint8) == sub1(to_bits(sizeof(uint8_t))));
static_assert(floored_log2(max_uint16) == sub1(to_bits(sizeof(uint16_t))));
static_assert(floored_log2(max_uint32) == sub1(to_bits(sizeof(uint32_t))));

// Also mixing in signed and unsigned types.

static_assert(floored_log2((1u << 0) + 0) == 0u);
static_assert(floored_log2((1u << 0) + 1) == 1u);
static_assert(floored_log2((1u << 1) + 0) == 1u);
static_assert(floored_log2((1u << 1) + 1) == 1u);
static_assert(floored_log2((1u << 2) + 0) == 2u);
static_assert(floored_log2((1u << 2) + 1) == 2u);
static_assert(floored_log2((1u << 3) + 0) == 3u);
static_assert(floored_log2((1u << 3) + 1) == 3u);
static_assert(floored_log2((1u << 4) + 0) == 4u);
static_assert(floored_log2((1u << 4) + 1) == 4u);
static_assert(floored_log2((1u << 5) + 0) == 5u);
static_assert(floored_log2((1u << 5) + 1) == 5u);
static_assert(floored_log2((1u << 6) + 0) == 6u);
static_assert(floored_log2((1u << 6) + 1) == 6u);
static_assert(floored_log2((1u << 7) + 0) == 7u);
static_assert(floored_log2((1u << 7) + 1) == 7u);
static_assert(floored_log2((1u << 8) + 0) == 8u);
static_assert(floored_log2((1u << 8) + 1) == 8u);
static_assert(floored_log2((1u << 9) + 0) == 9u);
static_assert(floored_log2((1u << 9) + 1) == 9u);
static_assert(floored_log2<int>((1 << 10) + 0) == 10);
static_assert(floored_log2<int>((1 << 10) + 1) == 10);
static_assert(floored_log2<int>((1 << 11) + 0) == 11);
static_assert(floored_log2<int>((1 << 11) + 1) == 11);
static_assert(floored_log2<int>((1 << 12) + 0) == 12);
static_assert(floored_log2<int>((1 << 12) + 1) == 12);
static_assert(floored_log2<int>((1 << 13) + 0) == 13);
static_assert(floored_log2<int>((1 << 13) + 1) == 13);
static_assert(floored_log2<int>((1 << 14) + 0) == 14);
static_assert(floored_log2<int>((1 << 14) + 1) == 14);
static_assert(floored_log2<int>((1 << 15) + 0) == 15);
static_assert(floored_log2<int>((1 << 15) + 1) == 15);
static_assert(floored_log2<int>((1 << 16) + 0) == 16);
static_assert(floored_log2<int>((1 << 16) + 1) == 16);
static_assert(floored_log2<int>((1 << 17) + 0) == 17);
static_assert(floored_log2<int>((1 << 17) + 1) == 17);
static_assert(floored_log2<int>((1 << 18) + 0) == 18);
static_assert(floored_log2<int>((1 << 18) + 1) == 18);
static_assert(floored_log2<int>((1 << 19) + 0) == 19);
static_assert(floored_log2<int>((1 << 19) + 1) == 19);
static_assert(floored_log2<int>((1 << 20) + 0) == 20);
static_assert(floored_log2<int>((1 << 20) + 1) == 20);
static_assert(floored_log2<int>((1 << 21) + 0) == 21);
static_assert(floored_log2<int>((1 << 21) + 1) == 21);
static_assert(floored_log2<int>((1 << 22) + 0) == 22);
static_assert(floored_log2<int>((1 << 22) + 1) == 22);
static_assert(floored_log2<int>((1 << 23) + 0) == 23);
static_assert(floored_log2<int>((1 << 23) + 1) == 23);
static_assert(floored_log2<int>((1 << 24) + 0) == 24);
static_assert(floored_log2<int>((1 << 24) + 1) == 24);
static_assert(floored_log2<int>((1 << 25) + 0) == 25);
static_assert(floored_log2<int>((1 << 25) + 1) == 25);
static_assert(floored_log2<int>((1 << 26) + 0) == 26);
static_assert(floored_log2<int>((1 << 26) + 1) == 26);
static_assert(floored_log2<int>((1 << 27) + 0) == 27);
static_assert(floored_log2<int>((1 << 27) + 1) == 27);
static_assert(floored_log2<int>((1 << 28) + 0) == 28);
static_assert(floored_log2<int>((1 << 28) + 1) == 28);
static_assert(floored_log2<int>((1 << 29) + 0) == 29);
static_assert(floored_log2<int>((1 << 29) + 1) == 29);
static_assert(floored_log2((1 << 30) + 0) == 30u);
static_assert(floored_log2((1 << 30) + 1) == 30u);
static_assert(floored_log2((1ll << 31) + 0) == 31u);
static_assert(floored_log2((1ll << 31) + 1) == 31u);

static_assert(floored_log2<int>(power2(15u)) == 15);
static_assert(floored_log2<unsigned>(power2(15u)) == 15u);

static_assert(floored_log2<int>(power(2, 15u)) == 15);
static_assert(floored_log2<unsigned>(power(2u, 15u)) == 15u);

static_assert(floored_log2(uint256_t((1u << 0) + 0)) == 0u);
static_assert(floored_log2(uint256_t((1u << 0) + 1)) == 1u);
static_assert(floored_log2(uint256_t((1u << 1) + 0)) == 1u);
static_assert(floored_log2(uint256_t((1u << 1) + 1)) == 1u);
static_assert(floored_log2(uint256_t((1u << 2) + 0)) == 2u);
static_assert(floored_log2(uint256_t((1u << 2) + 1)) == 2u);
static_assert(floored_log2(uint256_t((1u << 3) + 0)) == 3u);
static_assert(floored_log2(uint256_t((1u << 3) + 1)) == 3u);
static_assert(floored_log2(uint256_t((1u << 4) + 0)) == 4u);
static_assert(floored_log2(uint256_t((1u << 4) + 1)) == 4u);
static_assert(floored_log2(uint256_t((1u << 5) + 0)) == 5u);
static_assert(floored_log2(uint256_t((1u << 5) + 1)) == 5u);
static_assert(floored_log2(uint256_t((1u << 6) + 0)) == 6u);
static_assert(floored_log2(uint256_t((1u << 6) + 1)) == 6u);
static_assert(floored_log2(uint256_t((1u << 7) + 0)) == 7u);
static_assert(floored_log2(uint256_t((1u << 7) + 1)) == 7u);
static_assert(floored_log2(uint256_t((1u << 8) + 0)) == 8u);
static_assert(floored_log2(uint256_t((1u << 8) + 1)) == 8u);
static_assert(floored_log2(uint256_t((1u << 9) + 0)) == 9u);
static_assert(floored_log2(uint256_t((1u << 9) + 1)) == 9u);
static_assert(floored_log2<int>(uint256_t((1 << 10) + 0)) == 10);
static_assert(floored_log2<int>(uint256_t((1 << 10) + 1)) == 10);
static_assert(floored_log2<int>(uint256_t((1 << 11) + 0)) == 11);
static_assert(floored_log2<int>(uint256_t((1 << 11) + 1)) == 11);
static_assert(floored_log2<int>(uint256_t((1 << 12) + 0)) == 12);
static_assert(floored_log2<int>(uint256_t((1 << 12) + 1)) == 12);
static_assert(floored_log2<int>(uint256_t((1 << 13) + 0)) == 13);
static_assert(floored_log2<int>(uint256_t((1 << 13) + 1)) == 13);
static_assert(floored_log2<int>(uint256_t((1 << 14) + 0)) == 14);
static_assert(floored_log2<int>(uint256_t((1 << 14) + 1)) == 14);
static_assert(floored_log2<int>(uint256_t((1 << 15) + 0)) == 15);
static_assert(floored_log2<int>(uint256_t((1 << 15) + 1)) == 15);
static_assert(floored_log2<int>(uint256_t((1 << 16) + 0)) == 16);
static_assert(floored_log2<int>(uint256_t((1 << 16) + 1)) == 16);
static_assert(floored_log2<int>(uint256_t((1 << 17) + 0)) == 17);
static_assert(floored_log2<int>(uint256_t((1 << 17) + 1)) == 17);
static_assert(floored_log2<int>(uint256_t((1 << 18) + 0)) == 18);
static_assert(floored_log2<int>(uint256_t((1 << 18) + 1)) == 18);
static_assert(floored_log2<int>(uint256_t((1 << 19) + 0)) == 19);
static_assert(floored_log2<int>(uint256_t((1 << 19) + 1)) == 19);
static_assert(floored_log2<int>(uint256_t((1 << 20) + 0)) == 20);
static_assert(floored_log2<int>(uint256_t((1 << 20) + 1)) == 20);
static_assert(floored_log2<int>(uint256_t((1 << 21) + 0)) == 21);
static_assert(floored_log2<int>(uint256_t((1 << 21) + 1)) == 21);
static_assert(floored_log2<int>(uint256_t((1 << 22) + 0)) == 22);
static_assert(floored_log2<int>(uint256_t((1 << 22) + 1)) == 22);
static_assert(floored_log2<int>(uint256_t((1 << 23) + 0)) == 23);
static_assert(floored_log2<int>(uint256_t((1 << 23) + 1)) == 23);
static_assert(floored_log2<int>(uint256_t((1 << 24) + 0)) == 24);
static_assert(floored_log2<int>(uint256_t((1 << 24) + 1)) == 24);
static_assert(floored_log2<int>(uint256_t((1 << 25) + 0)) == 25);
static_assert(floored_log2<int>(uint256_t((1 << 25) + 1)) == 25);
static_assert(floored_log2<int>(uint256_t((1 << 26) + 0)) == 26);
static_assert(floored_log2<int>(uint256_t((1 << 26) + 1)) == 26);
static_assert(floored_log2<int>(uint256_t((1 << 27) + 0)) == 27);
static_assert(floored_log2<int>(uint256_t((1 << 27) + 1)) == 27);
static_assert(floored_log2<int>(uint256_t((1 << 28) + 0)) == 28);
static_assert(floored_log2<int>(uint256_t((1 << 28) + 1)) == 28);
static_assert(floored_log2<int>(uint256_t((1 << 29) + 0)) == 29);
static_assert(floored_log2<int>(uint256_t((1 << 29) + 1)) == 29);
static_assert(floored_log2(to_uintx(base16_hash("ffffffffffffffff"))) == 63u);
static_assert(floored_log2(to_uintx(base16_hash("010000000000000000"))) == 64u);
static_assert(floored_log2(to_uintx(base16_hash("ffffffffffffffffffffffffffffffff"))) == 127u);
static_assert(floored_log2(to_uintx(base16_hash("0100000000000000000000000000000000"))) == 128u);
static_assert(floored_log2(to_uintx(base16_hash("f000000000000000000000000000000000000000000000000000000000000000"))) == 255u);
static_assert(floored_log2(to_uintx(base16_hash("010000000000000000000000000000000000000000000000000000000000000000"))) == 256u);
static_assert(floored_log2(to_uintx(base16_hash("f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))) == 511u);
static_assert(floored_log2(to_uintx(base16_hash("0100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))) == 512u);

// floored_log256
static_assert(floored_log256(0xffffffffffffffffull) == 7);
static_assert(floored_log256(0xffffffffffffffull) == 6);
static_assert(floored_log256(0xffffffffffffull) == 5);
static_assert(floored_log256(0xffffffffffull) == 4);
static_assert(floored_log256(0xffffffffull) == 3);
static_assert(floored_log256(0xffffffull) == 2);
static_assert(floored_log256(0xffffull) == 1);
static_assert(floored_log256(0xffull) == 0);
static_assert(floored_log256(0x00ull) == 0);

// uint256_t
static_assert(floored_log256(uint256_t(0xffffffffffffffffull)) == 7);
static_assert(floored_log256(uint256_t(0xffffffffffffffull)) == 6);
static_assert(floored_log256(uint256_t(0xffffffffffffull)) == 5);
static_assert(floored_log256(uint256_t(0xffffffffffull)) == 4);
static_assert(floored_log256(uint256_t(0xffffffffull)) == 3);
static_assert(floored_log256(uint256_t(0xffffffull)) == 2);
static_assert(floored_log256(uint256_t(0xffffull)) == 1);
static_assert(floored_log256(uint256_t(0xffull)) == 0);
static_assert(floored_log256(uint256_t(0x00ull)) == 0);

// floored_log<base>
static_assert(floored_log<0u>(42) == 0u);
static_assert(floored_log<2u>((1u << 0) + 0) == 0u);
static_assert(floored_log<2u>((1u << 0) + 1) == 1u);
static_assert(floored_log<2u>((1u << 1) + 0) == 1u);
static_assert(floored_log<2u>((1u << 1) + 1) == 1u);
static_assert(floored_log<2u>((1u << 2) + 0) == 2u);
static_assert(floored_log<2u>((1u << 2) + 1) == 2u);
static_assert(floored_log<2u>((1u << 3) + 0) == 3u);
static_assert(floored_log<2u>((1u << 3) + 1) == 3u);
static_assert(floored_log<2u>((1u << 4) + 0) == 4u);
static_assert(floored_log<2u>((1u << 4) + 1) == 4u);
static_assert(floored_log<2u>((1u << 5) + 0) == 5u);
static_assert(floored_log<2u>((1u << 5) + 1) == 5u);
static_assert(floored_log<2u>((1u << 6) + 0) == 6u);
static_assert(floored_log<2u>((1u << 6) + 1) == 6u);
static_assert(floored_log<2u>((1u << 7) + 0) == 7u);
static_assert(floored_log<2u>((1u << 7) + 1) == 7u);
static_assert(floored_log<2u>((1u << 8) + 0) == 8u);
static_assert(floored_log<2u>((1u << 8) + 1) == 8u);
static_assert(floored_log<2u>((1u << 9) + 0) == 9u);
static_assert(floored_log<2u, size_t>((1u << 9) + 1) == 9u);
static_assert(floored_log<256u>(0xffffffffffffffffull) == 7);
static_assert(floored_log<256u>(0xffffffffffffffull) == 6);
static_assert(floored_log<256u>(0xffffffffffffull) == 5);
static_assert(floored_log<256u>(0xffffffffffull) == 4);
static_assert(floored_log<256u>(0xffffffffull) == 3);
static_assert(floored_log<256u>(0xffffffull) == 2);
static_assert(floored_log<256u>(0xffffull) == 1);
static_assert(floored_log<256u>(0xffull) == 0);
static_assert(floored_log<256u>(0x00ull) == 0);
static_assert(floored_log<2u, int>((1 << 10) + 0) == 10);
static_assert(floored_log<2u>((1 << 10) + 0) == 10);
static_assert(floored_log<2>(uint256_t{ 256 }) == 8u);

// uintx is not constexpr.

BOOST_AUTO_TEST_SUITE(logarithm_tests)

BOOST_AUTO_TEST_CASE(logarithm__ceilinged_log2_uintx__maximums__sizeof_maximum)
{
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx(max_uint8)), to_bits(sizeof(uint8_t)));
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx(max_uint16)), to_bits(sizeof(uint16_t)));
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx(max_uint32)), to_bits(sizeof(uint32_t)));
}

BOOST_AUTO_TEST_CASE(logarithm__ceilinged_log2_uintx__powers_of_2__expected)
{
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 32) + 0)), 32u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 32) + 1)), 33u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 63) + 0)), 63u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 63) + 1)), 64u);
}

BOOST_AUTO_TEST_CASE(logarithm__ceilinged_log2_uintx__pow2__identity)
{
    BOOST_REQUIRE_EQUAL(ceilinged_log2<int>(uintx(power2(15u))), 15);
    BOOST_REQUIRE_EQUAL(ceilinged_log2<unsigned>(uintx(power2(15u))), 15u);
}

BOOST_AUTO_TEST_CASE(logarithm__ceilinged_log2_uintx__power_2__identity)
{
    BOOST_REQUIRE_EQUAL(ceilinged_log2<int>(uintx(power(2, 15u))), 15);
    BOOST_REQUIRE_EQUAL(ceilinged_log2<unsigned>(uintx(power(2u, 15u))), 15u);
}

BOOST_AUTO_TEST_CASE(logarithm__floored_log2_uintx__maximums__sizeof_maximum_minus_one)
{
    BOOST_REQUIRE_EQUAL(floored_log2(uintx(max_uint8)), sub1(to_bits(sizeof(uint8_t))));
    BOOST_REQUIRE_EQUAL(floored_log2(uintx(max_uint16)), sub1(to_bits(sizeof(uint16_t))));
    BOOST_REQUIRE_EQUAL(floored_log2(uintx(max_uint32)), sub1(to_bits(sizeof(uint32_t))));
}

BOOST_AUTO_TEST_CASE(logarithm__floored_log2_uintx__powers_of_2__expected)
{
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 30) + 0)), 30u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 30) + 1)), 30u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 31) + 0)), 31u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 31) + 1)), 31u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1ull << 32) + 0)), 32u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1ull << 32) + 1)), 32u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1ull << 63) + 1)), 63u);
}

BOOST_AUTO_TEST_CASE(logarithm__floored_log2_uintx__pow2__identity)
{
    BOOST_REQUIRE_EQUAL(floored_log2<int>(uintx(power2(15u))), 15);
    BOOST_REQUIRE_EQUAL(floored_log2<unsigned>(uintx(power2(15u))), 15u);
}

BOOST_AUTO_TEST_CASE(logarithm__floored_log2_uintx__power_2__identity)
{
    BOOST_REQUIRE_EQUAL(floored_log2<int>(uintx(power(2, 15u))), 15);
    BOOST_REQUIRE_EQUAL(floored_log2<unsigned>(uintx(power(2u, 15u))), 15u);
}

BOOST_AUTO_TEST_SUITE_END()
