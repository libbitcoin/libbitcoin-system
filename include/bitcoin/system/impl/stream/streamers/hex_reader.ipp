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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BASE16_READER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BASE16_READER_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>

namespace libbitcoin {
namespace system {

// constructors
// ----------------------------------------------------------------------------

// protected
template <typename IStream>
hex_reader<IStream>::hex_reader() NOEXCEPT
  : base()
{
}

template <typename IStream>
hex_reader<IStream>::hex_reader(IStream& source) NOEXCEPT
  : base(source)
{
}

template <typename IStream>
hex_reader<IStream>::hex_reader(IStream& source,
    const base::memory_arena& arena) NOEXCEPT
  : base(source, arena)
{
}

// protected
// ----------------------------------------------------------------------------
// Overide all seek/read/limit operations.

template <typename IStream>
void hex_reader<IStream>::do_read_bytes(uint8_t* buffer,
    size_t size) NOEXCEPT
{
    BC_ASSERT(!is_null(buffer));
    
    // Scale for hex pairs and check limit.
    if (is_multiply_overflow(size, octet_width) ||
        this->limited(size * octet_width))
    {
        this->invalidate();
        return;
    }

    char chars[octet_width]{};
    const auto bytes = pointer_cast<uint8_t>(&chars[zero]);

    // Iteration avoids writing to dynamically-allocated buffer of size.
    for (auto octet = buffer; octet < std::next(buffer, size); ++octet)
    {
        base::do_read_bytes(bytes, octet_width);
        if (!is_base16(chars[0]) || !is_base16(chars[1]))
        {
            this->invalidate();
            return;
        }

        *octet = from_base16_characters(chars[0], chars[1]);
    }
}

template <typename IStream>
uint8_t hex_reader<IStream>::do_peek_byte() NOEXCEPT
{
    uint8_t octet{};
    do_read_bytes(&octet, sizeof(octet));
    do_rewind_bytes(sizeof(octet));
    return octet;
}

template <typename IStream>
void hex_reader<IStream>::do_skip_bytes(size_t size) NOEXCEPT
{
    if (is_multiply_overflow(size, octet_width) ||
        this->limited(size * octet_width))
    {
        this->invalidate();
        return;
    }

    base::do_skip_bytes(size * octet_width);
}

template <typename IStream>
void hex_reader<IStream>::do_rewind_bytes(size_t size) NOEXCEPT
{
    if (is_multiply_overflow(size, octet_width))
    {
        this->invalidate();
        return;
    }

    base::do_rewind_bytes(size * octet_width);
}

template <typename IStream>
size_t hex_reader<IStream>::get_read_position() NOEXCEPT
{
    // Position continues to represent logical bytes, so can be differenced.
    return base::get_read_position() / octet_width;
}

template <typename IStream>
void hex_reader<IStream>::set_limit(size_t size) NOEXCEPT
{
    if (is_multiply_overflow(size, octet_width))
    {
        this->invalidate();
        return;
    }

    base::set_limit(size * octet_width);
}

} // namespace system
} // namespace libbitcoin

#endif
