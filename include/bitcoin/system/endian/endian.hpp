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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_ENDIAN_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_ENDIAN_HPP

#include <bitcoin/system/endian/collection.hpp>
#include <bitcoin/system/endian/minimal.hpp>
#include <bitcoin/system/endian/nominal.hpp>
#include <bitcoin/system/endian/stream.hpp>
#include <bitcoin/system/endian/uintx.hpp>
#include <bitcoin/system/endian/uintx_t.hpp>
#include <bitcoin/system/endian/unchecked.hpp>

// These conversions are efficient, performing no buffer copies or reversals.
// array/uintx_t sizes are inferred by type, and vector/uintx by value.
// High order bits are padded when read (from) is insufficient.
// High order bits are ignored when write (to) is insufficient.
// uintx return can handle values of arbitrary size.

// STACK NUMBER DECODE / UTILITY
// Nominal (<Integer>/inferred):
// constexpr  unsigned_type<Size>          from_big_endian(data_array<Size>);
// constexpr  unsigned_type<Size>          from_little_endian(data_array<Size>);
// constexpr  Integral                     from_big_endian<Integral>(data_slice);
// constexpr  Integral                     from_little_endian<Integral>(data_slice);
// constexpr  data_array<sizeof(Integral)> to_big_endian{integral}(Integral);
// constexpr  data_array<sizeof(Integral)> to_little_endian{integral}(Integral);

// NETWORK
// Stream (<Integer>/stream):
// inline     Integral                     from_big_endian<Integral>{IStream}(IStream&);
// inline     Integral                     from_little_endian<Integral>{IStream}(IStream&);
// inline     void                         to_big_endian{OStream}(OStream&, Integer);
// inline     void                         to_little_endian{OStream}(OStream&, Integer);

// MERKLE
// Collection ([]):
// constexpr  void                         from_big_endian{[]}(Integer[], const uint8_t[]);
// constexpr  void                         from_little_endian{[]}(Integer[], const uint8_t[]);
// constexpr  void                         to_big_endian{[]}(uint8_t[], const Integer[]);
// constexpr  void                         to_little_endian{[]}(uint8_t[], const Integer[]);

// LIBRARY
// Uintx (dynamic int/data sizing): [uintx not constexpr]
// inline     uintx                        from_big_endian(data_chunk);
// inline     uintx                        from_little_endian(data_chunk);
// inline     data_chunk                   to_big_endian{uintx}(uintx);
// inline     data_chunk                   to_little_endian{uintx}(uintx);

// UNUSED
// Unchecked (Iterator): [database?]
// inline     Integral                     from_big_endian_unchecked<Integral>{}(Iterator);
// inline     Integral                     from_little_endian_unchecked<Integral>{}(Iterator);

// STACK NUMBER (chunk)
// Minimal (Size array / byte_width chunk): make integral for bytecast?
// constexpr  data_array<Size>             to_big_endian[_size]<Size>(Integer); %%
// constexpr  data_array<Size>             to_little_endian[_size]<Size>(Integer); %%
// VCONSTEXPR data_chunk(byte_width)       to_big_endian[[_chunk]_size](Integer); **
// VCONSTEXPR data_chunk(byte_width)       to_little_endian[[_chunk]_size](Integer); **

// POW
// Uintx_t (<Size>/array<Size>): [todo: constexpr]
// inline     exact_type<Size>             [uintx_]from_big_endian_chunk<Size>(data_slice); **
// inline     exact_type<Size>             [uintx_]from_little_endian_chunk<Size>(data_slice); **
// constexpr  exact_type<Size>             [uintx_]from_big_endian[_array]{Size}(data_array<Size>); %%
// constexpr  exact_type<Size>             [uintx_]from_little_endian[_array]{Size}(data_array<Size>); %%
// constexpr  data_array<Size>             to_big_endian<Size>(Integer); %%
// constexpr  data_array<Size>             to_little_endian<Size>(Integer); %%

// TODO:
// ------------------------------------------------------------------------------------------------
//
// STACK NUMBER ENCODE
// Chunked (<Size>/byte_width):
// inline     exact_type<Size>             from_big_endian_chunk<Size>([[data_slice]]);
// inline     exact_type<Size>             from_little_endian_chunk<Size>([[data_slice]]);
// VCONSTEXPR data_chunk(byte_width)       to_big_endian_chunk(Integer);
// VCONSTEXPR data_chunk(byte_width)       to_little_endian_chunk(Integer);
//
// POW
// Sized (<Size>): non-integrals:
// constexpr  exact_type<Size>             from_big_endian{Size}(data_array<Size>);
// constexpr  exact_type<Size>             from_little_endian{Size}(data_array<Size>);
// constexpr  data_array<Size>             to_big_endian<Size>(Integer);
// constexpr  data_array<Size>             to_little_endian<Size>(Integer);
// constexpr  data_array<Size>             to_big_endian<Size>(Integer);
// constexpr  data_array<Size>             to_little_endian<Size>(Integer);

// These are just aliases, move to the appropriate header.

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Hash conversions of corresponding integers.
template <uint32_t Bits,
    size_t Bytes = to_ceilinged_bytes(Bits)>
constexpr data_array<Bytes> to_array(const uintx_t<Bits>& value) NOEXCEPT
{
    return to_little_endian_size<Bytes>(value);
}

/// Integer conversions of corresponding hashes.
template <size_t Bytes,
    uint32_t Bits = to_bits<uint32_t>(Bytes)>
constexpr uintx_t<Bits> to_uintx(const data_array<Bytes>& hash) NOEXCEPT
{
    return uintx_from_little_endian_array<Bytes>(hash);
}

} // namespace system
} // namespace libbitcoin

#endif
