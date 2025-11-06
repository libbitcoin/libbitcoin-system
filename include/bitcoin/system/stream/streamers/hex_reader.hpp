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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BASE16_READER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BASE16_READER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/make_streamer.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>

namespace libbitcoin {
namespace system {

/// A base16 reader that accepts an istream.
template <typename IStream = std::istream>
class hex_reader
  : public virtual byte_reader<IStream>
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(hex_reader);
    using base = byte_reader<IStream>;

    /// Constructors.
    hex_reader(IStream& source) NOEXCEPT;
    hex_reader(IStream& source, const base::memory_arena& arena) NOEXCEPT;

protected:
    // For make_streamer<>.
    hex_reader() NOEXCEPT;
    template <class, template <class> class, class, class>
    friend class make_streamer;

    void do_read_bytes(uint8_t* buffer, size_t size) NOEXCEPT override;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/streamers/hex_reader.ipp>

#endif
