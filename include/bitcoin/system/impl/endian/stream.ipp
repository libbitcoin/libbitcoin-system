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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_STREAM_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_STREAM_IPP

#include <array>
#include <iostream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/nominal.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

#if defined(HAVE_BUFFERED_STREAM)

// non-buffered stream locals
// ----------------------------------------------------------------------------
// These are necessitated by the stream reader inside the iteration.

// This avoids a byteswap in exchange for a non-atomic write.
template <typename Integral, typename IStream>
inline Integral from_forward_stream(IStream& stream) NOEXCEPT
{
    using char_type = typename IStream::char_type;
    Integral value(0);
    constexpr auto bytes = sizeof(Integral);

    // TODO: flatten loop using if constexpr (or rely on compiler to do it).
    for (size_t byte = 0; byte < bytes; ++byte)
    {
        value <<= bits<char_type>;

        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        value |= possible_narrow_and_sign_cast<Integral>(stream.get());
        BC_POP_WARNING()
    }

    // Stream invalidated if read past end (and Integral is unspecified).
    return value;
}

// This avoids a byteswap in exchange for a non-atomic write.
template <typename Integral, typename OStream>
inline void to_reverse_stream(OStream& stream, Integral value) NOEXCEPT
{
    using char_type = typename OStream::char_type;
    constexpr auto bytes = sizeof(Integral);

    // TODO: flatten loop using if constexpr (or rely on compiler to do it).
    for (size_t byte = 0; byte < bytes; ++byte)
    {
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        stream.put(possible_sign_narrow_cast<char_type>(value));
        BC_POP_WARNING()

        value >>= bits<char_type>;
    }
}

// stream public
// ----------------------------------------------------------------------------

// Buffered stream should be more efficient, as the read is atomic and this
// still requires a byte swap, just as does the buffered read.
template <typename Integral, typename IStream,
    if_integral_integer<Integral>,
    if_same_size<typename IStream::char_type, uint8_t>>
inline Integral from_big_endian(IStream& stream) NOEXCEPT
{
    return native_from_big_end(from_forward_stream<Integral>(stream));
}

// This avoids a byteswap in exchange for a non-atomic write.
template <typename Integral, typename IStream,
    if_integral_integer<Integral>,
    if_same_size<typename IStream::char_type, uint8_t>>
inline Integral from_little_endian(IStream& stream) NOEXCEPT
{
    return native_from_little_end(from_forward_stream<Integral>(stream));
}

// This avoids a byteswap in exchange for a non-atomic write.
template <typename Integral, typename OStream,
    if_integral_integer<Integral>,
    if_same_size<typename OStream::char_type, uint8_t>>
inline void to_big_endian(OStream& stream, Integral value) NOEXCEPT
{
    to_reverse_stream(stream, native_to_little_end(value));
}

// Buffered stream should be more efficient, as the write is atomic and this
// still requires a byte swap, just as does the buffered write.
template <typename Integral, typename OStream,
    if_integral_integer<Integral>,
    if_same_size<typename OStream::char_type, uint8_t>>
inline void to_little_endian(OStream& stream, Integral value) NOEXCEPT
{
    to_reverse_stream(stream, native_to_big_end(value));
}

#else // HAVE_BUFFERED_STREAM

// iostreams are not costexpr.
// if stream.gcount() != size there should be a stream error.

// TODO: bytecast.
template <typename Integral, typename IStream = std::istream,
    if_integral_integer<Integral>,
    if_same_size<typename IStream::char_type, uint8_t>>
inline Integral from_big_endian(IStream& stream) NOEXCEPT
{
    constexpr auto size = sizeof(Integral);
    using char_type = typename IStream::char_type;
    std::array<char_type, size> buffer{};

    // nominal
    stream.read(buffer.data(), size);
    return from_big_endian<Integral>(buffer);
}

// TODO: bytecast, from_little_endian should be a no-op.
template <typename Integral, typename IStream = std::istream,
    if_integral_integer<Integral>,
    if_same_size<typename IStream::char_type, uint8_t>>
inline Integral from_little_endian(IStream& stream) NOEXCEPT
{
    constexpr auto size = sizeof(Integral);
    using char_type = typename IStream::char_type;
    std::array<char_type, size> buffer{};

    // nominal
    stream.read(buffer.data(), size);
    return from_little_endian<Integral>(buffer);
}

// TODO: bytecast.
template <typename Integral, typename OStream = std::ostream,
    if_integral_integer<Integral>,
    if_same_size<typename OStream::char_type, uint8_t>>
inline void to_big_endian(OStream& stream, Integral value) NOEXCEPT
{
    constexpr auto size = sizeof(Integral);
    using char_cptr = const typename OStream::char_type*;
    const auto& big_buffer = possible_pointer_cast<char_cptr>(
        to_big_endian(value).data());

    // nominal
    stream.write(big_buffer, size);
}

// TODO: bytecast, to_little_endian should be a no-op.
template <typename Integral, typename OStream = std::ostream,
    if_integral_integer<Integral>,
    if_same_size<typename OStream::char_type, uint8_t>>
inline void to_little_endian(OStream& stream, Integral value) NOEXCEPT
{
    constexpr auto size = sizeof(Integral);
    using char_cptr = const typename OStream::char_type*;
    const auto& little_buffer = possible_pointer_cast<char_cptr>(
        to_little_endian(value).data());

    // nominal
    stream.write(little_buffer, size);
}

#endif // HAVE_BUFFERED_STREAM

} // namespace system
} // namespace libbitcoin

#endif
