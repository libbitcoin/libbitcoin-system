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

 // en.cppreference.com/w/cpp/language/integer_literal

// There are integer literals of lesser size than int.
static_assert(is_same<decltype(0xff), signed int>());
static_assert(is_same<decltype(0xffU), unsigned int>());

// UNDESIRABLE SEAMS (unexpected size and sign)
// Default (unsuffixed) integer literals have the following signed types.
static_assert(is_same<decltype(0xff), signed int>());    // 1 (not 8)
static_assert(is_same<decltype(0xff'ff), signed int>());    // 2 (not 16)
static_assert(is_same<decltype(0xff'ff'ff), signed int>());    // 3
static_assert(is_same<decltype(0x7f'ff'ff'ff), signed int>());    // 4 (max is unsigned)
static_assert(is_same<decltype(0xff'ff'ff'ff), unsigned int>());     // 4 (max is unsigned)
static_assert(is_same<decltype(0xff'ff'ff'ff'ff), signed long long>()); // 5
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff), signed long long>()); // 6
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff), signed long long>()); // 7
static_assert(is_same<decltype(0x7f'ff'ff'ff'ff'ff'ff'ff), signed long long>()); // 8 (max is unsigned)
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());  // 8 (max is unsigned)

// All literal representations correspond in type (who uses octal?).
static_assert(is_same<decltype(0xff), decltype(255)>());
static_assert(is_same<decltype(0xff'ff), decltype(65'535)>());
static_assert(is_same<decltype(0xff'ff'ff), decltype(16'777'215)>());
static_assert(is_same<decltype(0xff'ff'ff'ff), decltype(4'294'967'295u)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff), decltype(1'099'511'627'775)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff), decltype(281'474'976'710'655)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff), decltype(72'057'594'037'927'935)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ff), decltype(18'446'744'073'709'551'615)>());
static_assert(is_same<decltype(0xff), decltype(0b1111'1111)>());
static_assert(is_same<decltype(0xff'ff), decltype(0b1111'1111'1111'1111)>());
static_assert(is_same<decltype(0xff'ff'ff), decltype(0b1111'1111'1111'1111'1111'1111)>());
static_assert(is_same<decltype(0xff'ff'ff'ff), decltype(0b1111'1111'1111'1111'1111'1111'1111'1111)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff), decltype(0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff), decltype(0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff), decltype(0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111)>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ff), decltype(0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111)>());

// All literal representations correspond in value.
static_assert(0xff == 255);
static_assert(0xff'ff == 65'535);
static_assert(0xff'ff'ff == 16'777'215);
static_assert(0xff'ff'ff'ff == 4'294'967'295); // unsigned
static_assert(0xff'ff'ff'ff'ff == 1'099'511'627'775);
static_assert(0xff'ff'ff'ff'ff'ff == 281'474'976'710'655);
static_assert(0xff'ff'ff'ff'ff'ff'ff == 72'057'594'037'927'935);
static_assert(0xff'ff'ff'ff'ff'ff'ff'ff == 18'446'744'073'709'551'615); // unsigned
static_assert(0xff == 0b1111'1111);
static_assert(0xff'ff == 0b1111'1111'1111'1111);
static_assert(0xff'ff'ff == 0b1111'1111'1111'1111'1111'1111);
static_assert(0xff'ff'ff'ff == 0b1111'1111'1111'1111'1111'1111'1111'1111);
static_assert(0xff'ff'ff'ff'ff == 0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111);
static_assert(0xff'ff'ff'ff'ff'ff == 0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111);
static_assert(0xff'ff'ff'ff'ff'ff'ff == 0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111);
static_assert(0xff'ff'ff'ff'ff'ff'ff'ff == 0b1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111'1111);

// UNDESIRABLE SEAM (unexpected overflow behavior, resulting from unexpected size and sign)
// As all literal representations are positive (- is an operator),
// When negating a (positive) literal, the type width controls overflow.
// A literal without the high bit set cannot correspond to a negative value.
static_assert(0xff != -1); // POSITIVE
static_assert(0xff'ff != -1); // POSITIVE
static_assert(0xff'ff'ff != -1); // POSITIVE
static_assert(0xff'ff'ff'ff == -1); // unsigned int overflow to corresponding negative
static_assert(0xff'ff'ff'ff'ff != -1); // POSITIVE
static_assert(0xff'ff'ff'ff'ff'ff != -1); // POSITIVE
static_assert(0xff'ff'ff'ff'ff'ff'ff != -1); // POSITIVE
static_assert(0xff'ff'ff'ff'ff'ff'ff'ff == -1); // unsigned long long overflow to corresponding negative

// There are no negative integer literals.
// Applying a unary operator is an operation, unary operators do not change sign.
// Literals are not promotable by unary operators (as they are already promoted).
static_assert(is_same<decltype(+0xff), decltype(0xff)>());
static_assert(is_same<decltype(+0xff'ff), decltype(0xff'ff)>());
static_assert(is_same<decltype(+0xff'ff'ff), decltype(0xff'ff'ff)>());
static_assert(is_same<decltype(+0xff'ff'ff'ff), decltype(0xff'ff'ff'ff)>());
static_assert(is_same<decltype(+0xff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(+0xff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(+0xff'ff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(+0xff'ff'ff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(-0xff), decltype(0xff)>());
static_assert(is_same<decltype(-0xff'ff), decltype(0xff'ff)>());
static_assert(is_same<decltype(-0xff'ff'ff), decltype(0xff'ff'ff)>());
//static_assert(is_same<decltype(-0xff'ff'ff'ff), decltype(0xff'ff'ff'ff)>());
static_assert(is_same<decltype(-0xff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(-0xff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(-0xff'ff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff'ff)>());
//static_assert(is_same<decltype(-0xff'ff'ff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(~0xff), decltype(0xff)>());
static_assert(is_same<decltype(~0xff'ff), decltype(0xff'ff)>());
static_assert(is_same<decltype(~0xff'ff'ff), decltype(0xff'ff'ff)>());
static_assert(is_same<decltype(~0xff'ff'ff'ff), decltype(0xff'ff'ff'ff)>());
static_assert(is_same<decltype(~0xff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(~0xff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(~0xff'ff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff'ff)>());
static_assert(is_same<decltype(~0xff'ff'ff'ff'ff'ff'ff'ff), decltype(0xff'ff'ff'ff'ff'ff'ff'ff)>());

// Non-suffixed literals are signed int or ll (with the exception of max int/ll).
static_assert(is_same<decltype(0xff), signed int>());      // 1 (not 8)
static_assert(is_same<decltype(0xff'ff), signed int>());      // 2 (not 16)
static_assert(is_same<decltype(0xff'ff'ff), signed int>());      // 3
static_assert(is_same<decltype(0x7f'ff'ff'ff), signed int>());      // 4 (unless max)
static_assert(is_same<decltype(0xff'ff'ff'ff), unsigned int>());       // 4 (max is unsigned)
static_assert(is_same<decltype(0xff'ff'ff'ff'ff), signed long long>());   // 5
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff), signed long long>());   // 6
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff), signed long long>());   // 7
static_assert(is_same<decltype(0x7f'ff'ff'ff'ff'ff'ff'ff), signed long long>());   // 8 (unless max)
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());    // 8 (max is unsigned)

// Use upper case suffixes ("l" looks like "1").
// Integer suffixes L and LL may only increase size (not change sign).
static_assert(is_same<decltype(0xffL), signed long>());
static_assert(is_same<decltype(0xff'ffL), signed long>());
static_assert(is_same<decltype(0xff'ff'ffL), signed long>());
static_assert(is_same<decltype(0x7f'ff'ff'ffL), signed long>()); // limited
static_assert(is_same<decltype(0xff'ff'ff'ff'ffL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ffL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ffL), signed long long>());
static_assert(is_same<decltype(0x7f'ff'ff'ff'ff'ff'ff'ffL), signed long long>()); // limited
static_assert(is_same<decltype(0xffLL), signed long long>());
static_assert(is_same<decltype(0xff'ffLL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ffLL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ffLL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ffLL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ffLL), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ffLL), signed long long>());
static_assert(is_same<decltype(0x7f'ff'ff'ff'ff'ff'ff'ffLL), signed long long>()); // limited

// Integer suffix U always assures unsigned and does not change size.
static_assert(is_same<decltype(0xffU), unsigned int>());
static_assert(is_same<decltype(0xff'ffU), unsigned int>());
static_assert(is_same<decltype(0xff'ff'ffU), unsigned int>());
static_assert(is_same<decltype(0xff'ff'ff'ffU), unsigned int>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ffU), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ffU), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ffU), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ffU), unsigned long long>());
static_assert(is_same<decltype(0xffUL), unsigned long>());
static_assert(is_same<decltype(0xff'ffUL), unsigned long>());
static_assert(is_same<decltype(0xff'ff'ffUL), unsigned long>());
static_assert(is_same<decltype(0xff'ff'ff'ffUL), unsigned long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ffUL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ffUL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ffUL), unsigned long long>());
static_assert(is_same<decltype(0x7f'ff'ff'ff'ff'ff'ff'ffUL), unsigned long long>()); // limited
static_assert(is_same<decltype(0xffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ffULL), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ffULL), unsigned long long>());

// Note that 0xffffffff and 0xffffffffffffffff are domain limited.
// 0xffffffffffffffff cannot in any way be represented as a signed literal.
// 0xffffffff cannot in any way be represented as a 32 bit signed literal.
// 0xffffffff can be explicitly promoted to LL, becoming a 64 bit signed literal.
static_assert(0xff'ff'ff'ffLL == 0x00'00'00'00'ff'ff'ff'ff);
static_assert(is_same<decltype(0xff'ff'ff'ffLL), signed long long>());

// UNDESIRABLE SEAM (unexpected sign change based on value)
// Note that any literal of 32 bits will become unsigned iff its high bit is set.
static_assert(is_same<decltype(0xff), signed int>());
static_assert(is_same<decltype(0xff'ff), signed int>());
static_assert(is_same<decltype(0xff'ff'ff), signed int>());
static_assert(is_same<decltype(0x7f'ff'ff'ff), signed int>());
static_assert(is_same<decltype(0x8f'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0x9f'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0xaf'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0xbf'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0xcf'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0xdf'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0xef'ff'ff'ff), unsigned int>());
static_assert(is_same<decltype(0xff'ff'ff'ff), unsigned int>());

// UNDESIRABLE SEAM (unexpected sign change based on value)
// Note that any literal of 64 bits will become unsigned iff its high bit is set.
static_assert(is_same<decltype(0xff'ff'ff'ff'ff), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff), signed long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff), signed long long>());
static_assert(is_same<decltype(0x7f'ff'ff'ff'ff'ff'ff'ff), signed long long>());
static_assert(is_same<decltype(0x8f'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0x9f'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0xaf'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0xbf'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0xcf'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0xdf'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0xef'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());
static_assert(is_same<decltype(0xff'ff'ff'ff'ff'ff'ff'ff), unsigned long long>());

// These are the relations to numbered integral sizes.
static_assert(sizeof(0xff) >= sizeof(int8_t));
static_assert(sizeof(0xffff) >= sizeof(int16_t));
static_assert(sizeof(0xffff'ffff) >= sizeof(int32_t));
static_assert(sizeof(0xffff'ffff'ffff'ffff) >= sizeof(int64_t));

// Custom literals cast to the specified positive type.
static_assert(is_same<decltype(0xff_i8), int8_t>());
static_assert(is_same<decltype(0xff_i16), int16_t>());
static_assert(is_same<decltype(0xff_i32), int32_t>());
static_assert(is_same<decltype(0xff_i64), int64_t>());
static_assert(is_same<decltype(0xff_u8), uint8_t>());
static_assert(is_same<decltype(0xff_u16), uint16_t>());
static_assert(is_same<decltype(0xff_u32), uint32_t>());
static_assert(is_same<decltype(0xff_ni8), int8_t>());
static_assert(is_same<decltype(0xff_ni16), int16_t>());
static_assert(is_same<decltype(0xff_ni32), int32_t>());
static_assert(is_same<decltype(0xff_ni64), int64_t>());
static_assert(is_same<decltype(0xff_nu8), uint8_t>());
static_assert(is_same<decltype(0xff_nu16), uint16_t>());
static_assert(is_same<decltype(0xff_nu32), uint32_t>());
static_assert(is_same<decltype(0xff_nu64), uint64_t>());
static_assert(is_same<decltype(0xff_size), size_t>());
static_assert(is_same<decltype(0xff_nsize), size_t>());

// Custom literal aliases (for vertical alignment).
static_assert(is_same<decltype(0xff_i08), decltype(0xff_i8)>());
static_assert(is_same<decltype(0xff_u08), decltype(0xff_u8)>());
static_assert(is_same<decltype(0xff_ni8), decltype(0xff_ni8)>());
static_assert(is_same<decltype(0xff_nu8), decltype(0xff_nu8)>());

// Oversized literals are truncated in type (64 bit cannot be overflowed).
static_assert(is_same<decltype(0x42ff_i8), int8_t>());
static_assert(is_same<decltype(0x42ffff_i16), int16_t>());
static_assert(is_same<decltype(0x42ffffffff_i32), int32_t>());
static_assert(is_same<decltype(0xffffffffffffffff_i64), int64_t>());
static_assert(is_same<decltype(0x42ff_u8), uint8_t>());
static_assert(is_same<decltype(0x42ffff_u16), uint16_t>());
static_assert(is_same<decltype(0x42ffffffff_u32), uint32_t>());
static_assert(is_same<decltype(0xffffffffffffffff_u64), uint64_t>());

// size_t truncation is dependent upon sizeof(size_t).
static_assert(is_same<decltype(0xffffffffffffffff_size), size_t>());

// Oversized literals cannot be represented (64 bit cannot be oversized).
// size_t overflow is compiler-conditional, so use should be limited to 32 bits.
////static_assert(0x42ff_i8 == 0xff_i8);
////static_assert(0x42ffff_i16 == 0xffff_i16);
////static_assert(0x42ffffffff_i32 == 0xffffffff_i32);
static_assert(0xffffffffffffffff_i64 == 0xffffffffffffffff_i64);
////static_assert(0x42ff_u8 == 0xff_u8);
////static_assert(0x42ffff_u16 == 0xffff_u16);
////static_assert(0x42ffffffff_u32 == 0xffffffff_u32);
static_assert(0xffffffffffffffff_u64 == 0xffffffffffffffff_u64);
////static_assert(0xffffffffffffffff_size == 0xffffffffffffffff_size);

// Hexadecimal literals.
static_assert(0x0100_i16 == 256);
static_assert(0x7f_i8 == 127);
static_assert(0x70_i8 == 112);
static_assert(0x60_i8 == 96);
static_assert(0x50_i8 == 80);
static_assert(0x40_i8 == 64);
static_assert(0x30_i8 == 48);
static_assert(0x20_i8 == 32);
static_assert(0x10_i8 == 16);
static_assert(0x09_i8 == 9);
static_assert(0x08_i8 == 8);
static_assert(0x07_i8 == 7);
static_assert(0x06_i8 == 6);
static_assert(0x05_i8 == 5);
static_assert(0x04_i8 == 4);
static_assert(0x03_i8 == 3);
static_assert(0x02_i8 == 2);
static_assert(0x01_i8 == 1);
static_assert(0x00_i8 == 0);
static_assert(0xff_i8 == -1);
static_assert(0xfe_i8 == -2);
static_assert(0xfd_i8 == -3);
static_assert(0xfc_i8 == -4);
static_assert(0xfb_i8 == -5);
static_assert(0xfa_i8 == -6);
static_assert(0xf9_i8 == -7);
static_assert(0xf8_i8 == -8);
static_assert(0xf7_i8 == -9);
static_assert(0xf0_i8 == -16);
static_assert(0xe0_i8 == -32);
static_assert(0xd0_i8 == -48);
static_assert(0xc0_i8 == -64);
static_assert(0xb0_i8 == -80);
static_assert(0xa0_i8 == -96);
static_assert(0x90_i8 == -112);
static_assert(0x80_i8 == -128);
static_assert(0xff00_i16 == -256);
static_assert(is_same<decltype(0x42_i8), int8_t>());

constexpr auto foo = -1;
constexpr auto bar = 0xff_i8;
static_assert(foo == bar);

// Binary literals.
static_assert(0b0111'1111_i8 == 127);
static_assert(0b0111'0000_i8 == 112);
static_assert(0b0110'0000_i8 == 96);
static_assert(0b0101'0000_i8 == 80);
static_assert(0b0100'0000_i8 == 64);
static_assert(0b0011'0000_i8 == 48);
static_assert(0b0010'0000_i8 == 32);
static_assert(0b0001'0000_i8 == 16);
static_assert(0b0000'0001_i8 == 1);
static_assert(0b0000'0000_i8 == 0);
static_assert(is_same<decltype(0b0101'0101_i8), int8_t>());

// Decimal literals okay, but...
static_assert(127_i8 == 127);
static_assert(112_i8 == 112);
static_assert( 96_i8 == 96);
static_assert( 80_i8 == 80);
static_assert( 64_i8 == 64);
static_assert( 48_i8 == 48);
static_assert( 32_i8 == 32);
static_assert( 16_i8 == 16);
static_assert(  1_i8 == 1);
static_assert(  0_i8 == 0);
static_assert(is_same<decltype(42_i8), int8_t>());

// ...negatives are not decimal literals, they are (promoting) operators!
// While they may appear to represent the value in the type, they do not.
// And the minimum value cannot be expressed (maximum positive exceeded).
// There is *no way* to represent a negative value using a decimal literal.
static_assert(  -1_i8 == -1);
static_assert( -16_i8 == -16);
static_assert( -32_i8 == -32);
static_assert( -48_i8 == -48);
static_assert( -64_i8 == -64);
static_assert( -80_i8 == -80);
static_assert( -96_i8 == -96);
static_assert(-112_i8 == -112);
////static_assert(-128_i8 == -128); // <== undefined negation (but in range)
static_assert(is_same<decltype(-42_i8), signed int>()); // <==

// Instead use negated literals, which preserve type/domain.

// Negative representation (signed).
static_assert(0_ni8 == 0);
static_assert(1_ni8 == -1);
static_assert(2_ni8 == -2);
static_assert(3_ni8 == -3);
static_assert(126_ni8 == -126);
static_assert(127_ni8 == -127);
static_assert(128_ni8 == -128); // <== defined (no negation)
////static_assert(129_ni8 == -129); // <== correctly out of range
static_assert(is_same<decltype(42_ni8), int8_t>()); // <==

// Twos complement representation (unsigned).
static_assert(0_nu8 == 0);
static_assert(1_nu8 == 255);
static_assert(2_nu8 == 254);
static_assert(3_nu8 == 253);
static_assert(126_nu8 == 130);
static_assert(127_nu8 == 129);
static_assert(128_nu8 == 128);
////static_assert(129_nu8 == 127);
static_assert(is_same<decltype(1_nu8), uint8_t>());

// Literal bounds.

// Domain bounds (_i8).
static_assert(0x7f_i8 == sub1(to_half(power2<int16_t>(8))));
static_assert(0x01_i8 == 1);
static_assert(0x00_i8 == 0);
static_assert(0xff_i8 == 1_ni8);
static_assert(0x80_i8 == 128_ni8);

// Domain bounds (_i16).
static_assert(0x7fff_i16 == sub1(to_half(power2<int32_t>(16))));
static_assert(0x0001_i16 == 1);
static_assert(0x0000_i16 == 0);
static_assert(0xffff_i16 == 1_ni16);
static_assert(0x8000_i16 == 32'768_ni16);

// Domain bounds (_i32).
static_assert(0x7fffffff_i32 == sub1(to_half(power2<int64_t>(32))));
static_assert(0x00000001_i32 == 1);
static_assert(0x00000000_i32 == 0);
static_assert(0xffffffff_i32 == 1_ni64);
static_assert(0x80000000_i32 == 2'147'483'648_ni64);

// Domain bounds (_i64).
static_assert(0x7fffffffffffffff_i64 == sub1(to_half(power2<uint128_t>(64))));
static_assert(0x0000000000000001_i64 == 1);
static_assert(0x0000000000000000_i64 == 0);
static_assert(0xffffffffffffffff_i64 == 1_ni64);
static_assert(0x8000000000000000_i64 == 9'223'372'036'854'775'808_ni64);

// Domain bounds (_u8).
static_assert(0xff_u8 == sub1(power2<uint16_t>(8)));
static_assert(0x01_u8 == 1);
static_assert(0x00_u8 == 0);

// Domain bounds (_u16).
static_assert(0xffff_u16 == sub1(power2<uint32_t>(16)));
static_assert(0x0001_u16 == 1);
static_assert(0x0000_u16 == 0);

// Domain bounds (_u32).
static_assert(0xffffffff_u32 == sub1(power2<uint64_t>(32)));
static_assert(0x00000001_u32 == 1);
static_assert(0x00000000_u32 == 0);

// Domain bounds (_u64).
static_assert(0xffffffffffffffff_u64 == sub1(power2<uint128_t>(64)));
static_assert(0x0000000000000001_u64 == 1);
static_assert(0x0000000000000000_u64 == 0);

// Domain bounds (_ni8).
static_assert(0x00_ni8 == 0);
static_assert(0x01_ni8 == -1);
static_assert(0x80_ni8 == -to_half(power2<int16_t>(8)));

// Domain bounds (_ni16).
static_assert(0x0000_ni16 == 0);
static_assert(0x0001_ni16 == -1);
static_assert(0x8000_ni16 == -to_half(power2<int32_t>(16)));

// Domain bounds (_ni32).
static_assert(0x00000000_ni32 == 0);
static_assert(0x00000001_ni32 == -1);
static_assert(0x80000000_ni32 == -to_half(power2<int64_t>(32)));

// Domain bounds (_ni64).
static_assert(0x0000000000000000_ni64 == 0);
static_assert(0x0000000000000001_ni64 == -1);
static_assert(0x8000000000000000_ni64 == twos_complement(to_half(power2<uint128_t>(64))));

BOOST_AUTO_TEST_SUITE(literals_tests)

BOOST_AUTO_TEST_CASE(literals__positive__invalid_u32__overflow_exception)
{
    BOOST_REQUIRE_EQUAL(positive<uint32_t>(0x000fbaadf00d_u64), 0_u32);
}

BOOST_AUTO_TEST_CASE(literals__positive__1_u8__expected)
{
    static_assert(positive<uint8_t>(0xff_u64) == 0xff_u8);
    BOOST_REQUIRE_EQUAL(positive<uint8_t>(0xff_u64), 0xff_u8);
}

BOOST_AUTO_TEST_CASE(literals__positive__1_i8__expected)
{
    static_assert(positive<int16_t>(0x7fff_u64) == 0x7fff_u16);
    BOOST_REQUIRE_EQUAL(positive<int16_t>(0x7fff_u64), 0x7fff_u16);
}

BOOST_AUTO_TEST_CASE(literals__negative__1_nu8__expected)
{
    static_assert(negative<uint32_t>(12'345'678_u64) == 12'345'678_nu32);
    BOOST_REQUIRE_EQUAL(negative<uint32_t>(12'345'678_u64), 12'345'678_nu32);
}

BOOST_AUTO_TEST_CASE(literals__negative__1_ni8__expected)
{
    static_assert(negative<int64_t>(12'345'678'900_u64) == 12'345'678'900_ni64);
    BOOST_REQUIRE_EQUAL(negative<int64_t>(12'345'678'900_u64), 12'345'678'900_ni64);
}

// Static msvc++20 compiler incorrectly suggests const may be made constexpr.

// TODO: make custom literals consteval (vs. constexpr) when full C++ compiler
// support is available (clang), to close this runtime hole. Otherwise always
// use the custom literals as constexpr and otherwise expect a runtime abort
// when they are *evaluated*.

// Each test individually verified and then disabled, as they process terminate.
// _size/_size tests verified in 32 bit build.

////BOOST_AUTO_TEST_CASE(literals__i8__literal_overflow__terminate)
////{
////    // thread safe
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _i8 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ff_i8, -1);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__i16__literal_overflow__terminate)
////{
////    // thread safe
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _i16 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ffff_i16, -1);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__i32__literal_overflow__terminate)
////{
////    // thread safe
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _i32 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ffffffff_i32, -1);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__u8__literal_overflow__terminate)
////{
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _u8 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ff_u8, 0xffU);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__u16__literal_overflow__terminate)
////{
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _u16 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ffff_u16, 0xffffU);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__u32__literal_overflow__terminate)
////{
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _u32 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ffffffff_u32, 0xffffffffU);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__size__literal_overflow__terminate)
////{
////    // size_t(64) cannot be overflowed due to limit of integer literal expression.
////    if constexpr (sizeof(size_t) == sizeof(uint32_t))
////    {
////        const auto saved = std::set_terminate([]()
////        {
////            // The test process will terminate after this call.
////            BOOST_REQUIRE_MESSAGE(false, "non-const _size literal overflow");
////        });
////
////        // These would be equal if they failed to abort, so fail here on not equal.
////        BOOST_REQUIRE_NE(0x42ffffffff_size, 0xffffffffU);
////        std::set_terminate(saved);
////    }
////}
////
////BOOST_AUTO_TEST_CASE(literals__i08__literal_overflow__terminate)
////{
////    // thread safe
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _i08 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ff_i08, -1);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__u08__literal_overflow__terminate)
////{
////    const auto saved = std::set_terminate([]()
////    {
////        // The test process will terminate after this call.
////        BOOST_REQUIRE_MESSAGE(false, "non-const _u08 literal overflow");
////    });
////
////    // These would be equal if they failed to abort, so fail here on not equal.
////    BOOST_REQUIRE_NE(0x42ff_u08, 0xffU);
////    std::set_terminate(saved);
////}
////
////BOOST_AUTO_TEST_CASE(literals__siz__literal_overflow__terminate)
////{
////    // size_t(64) cannot be overflowed due to limit of integer literal expression.
////    if constexpr (sizeof(size_t) == sizeof(uint32_t))
////    {
////        const auto saved = std::set_terminate([]()
////        {
////            // The test process will terminate after this call.
////            BOOST_REQUIRE_MESSAGE(false, "non-const _siz literal overflow");
////        });
////
////        // These would be equal if they failed to abort, so fail here on not equal.
////        BOOST_REQUIRE_NE(0x42ffffffff_siz, 0xffffffffU);
////        std::set_terminate(saved);
////    }
////}

BOOST_AUTO_TEST_SUITE_END()
