/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BASE16_WRITER_IPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BASE16_WRITER_IPP

#include <algorithm>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>

namespace libbitcoin {
namespace system {

// constructors
// ----------------------------------------------------------------------------

// protected
template <typename OStream>
hex_writer<OStream>::hex_writer() NOEXCEPT
  : base()
{
}


template <typename OStream>
hex_writer<OStream>::hex_writer(OStream& sink) NOEXCEPT
  : base(sink)
{
}

// protected
// ----------------------------------------------------------------------------

template <typename OStream>
void hex_writer<OStream>::do_write_bytes(const uint8_t* data,
    size_t size) NOEXCEPT
{
    char chars[two];
    const auto bytes = pointer_cast<uint8_t>(&chars[zero]);

    // Iteration avoids writing to dynamically-allocated buffer of size.
    std::for_each_n(data, size, [&](uint8_t octet) NOEXCEPT
    {
        chars[0] = to_base16_hi_character(octet);
        chars[1] = to_base16_lo_character(octet);
        base::do_write_bytes(bytes, two);
    });
}

} // namespace system
} // namespace libbitcoin

#endif
