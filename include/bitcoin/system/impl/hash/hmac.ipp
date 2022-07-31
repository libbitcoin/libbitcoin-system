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
#ifndef LIBBITCOIN_SYSTEM_HASH_HMAC_IPP
#define LIBBITCOIN_SYSTEM_HASH_HMAC_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/accumulator.hpp>

// Based on:
// datatracker.ietf.org/doc/html/rfc2104
// [Krawczyk, Bellare, Canetti]

namespace libbitcoin {
namespace system {

#define TEMPLATE template <typename Algorithm>
#define CLASS hmac<Algorithm>

// hmac accumulator
// ---------------------------------------------------------------------------

TEMPLATE
inline CLASS::
hmac(const data_slice& key) NOEXCEPT
{
    constexpr auto block_bytes = array_count<typename Algorithm::block_t>;
    constexpr auto digest_bytes = array_count<typename Algorithm::digest_t>;

    // rfc2104
    // K if K is not larger than block size.
    if (key.size() <= block_bytes)
    {
        xor_key(key.data(), key.size());
        return;
    }

    // rfc2104
    // H(K) if K is larger than block size.
    accumulator<Algorithm> accumulator{};
    accumulator.write(key);
    xor_key(accumulator.flush().data(), digest_bytes);
}

TEMPLATE
inline void CLASS::
write(const data_slice& data) NOEXCEPT
{
    // rfc2104
    // append the stream of data 'text' to the B byte string.
    inner_.write(data);
}

TEMPLATE
inline typename CLASS::digest_t
CLASS::
flush() NOEXCEPT
{
    // rfc2104
    // append the H result ... to the B byte string.
    outer_.write(inner_.flush());

    // rfc2104
    // apply H to the stream ...and output the result.
    return outer_.flush();
}

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// protected/static
TEMPLATE
constexpr typename CLASS::block_t&
CLASS::
xor_n(block_t& pad, const byte_t* key, size_t size) NOEXCEPT
{

    // rfc2104
    // append zeros to the end of K to create a B byte string [skip].
    // XOR (bitwise exclusive-OR) the B byte string ... with [ipad/opad].
    // [not xoring beyond K length is the same as xoring that with zero].
    for (size_t i = 0; i < size; ++i)
        pad[i] ^= key[i];

    return pad;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

// protected/static
TEMPLATE
CONSTEVAL typename CLASS::block_t
hmac<Algorithm>::
inner_pad() NOEXCEPT
{
    // rfc2104
    // ipad = the byte 0x36 repeated B times
    block_t ipad{};
    ipad.fill(0x36_u8);
    return ipad;
}

// protected/static
TEMPLATE
CONSTEVAL typename CLASS::block_t
hmac<Algorithm>::
outer_pad() NOEXCEPT
{
    // rfc2104
    // opad = the byte 0x5C repeated B times.
    block_t opad{};
    opad.fill(0x5c_u8);
    return opad;
}

// protected
TEMPLATE
inline void CLASS::
xor_key(const byte_t* key, size_t size) NOEXCEPT
{
    constexpr auto ipad = inner_pad();
    constexpr auto opad = outer_pad();

    // rfc2104
    // XOR (bitwise exclusive-OR) the B byte string ... with ipad.
    auto pad = ipad;
    inner_.write(xor_n(pad, key, size));

    // rfc2104
    // XOR (bitwise exclusive-OR) the B byte string ... with opad.
    pad = opad;
    outer_.write(xor_n(pad, key, size));
}

// finalized authentication code
// ---------------------------------------------------------------------------

TEMPLATE
inline typename CLASS::digest_t CLASS::
code(const data_slice& data, const data_slice& key) NOEXCEPT
{
    // rfc2104
    // H(K XOR opad, H(K XOR ipad, text))
    hmac<Algorithm> buffer{ key };
    buffer.write(data);
    return buffer.flush();
}

#undef CLASS
#undef TEMPLATE

} // namespace system
} // namespace libbitcoin

#endif
