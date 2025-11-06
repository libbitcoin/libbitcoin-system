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
#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_FLIPPER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BYTE_FLIPPER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/make_streamer.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>
#include <bitcoin/system/stream/streamers/interfaces/byteflipper.hpp>

// The only multiple inheritance conflicts are resolved below.
BC_PUSH_WARNING(DIAMOND_INHERITANCE)

namespace libbitcoin {
namespace system {

/// A byte reader/writer that accepts an iostream.
template <typename IOStream = std::iostream>
class byte_flipper
  : public byte_reader<IOStream>,
    public byte_writer<IOStream>,
    public virtual byteflipper
{
public:
    DEFAULT_COPY_MOVE(byte_flipper);
    using ibase = byte_reader<IOStream>;
    using obase = byte_writer<IOStream>;

    byte_flipper(IOStream& stream) NOEXCEPT
      : ibase(stream), obase(stream)
    {
        // Base classes only capture pointers, both to the same stream.
    }

    /// These overrides eliminate ambiguity resulting from diamond inheritance.

    // Two base destructor calls order is unimportant (only writes flush).
    ~byte_flipper() override = default;

    operator bool() const NOEXCEPT override
    {
        // Rely on reader implementation, both are trivial and identical.
        return ibase::operator bool();
    }

    bool operator!() const NOEXCEPT override
    {
        // Rely on reader implementation, both are trivial and identical.
        return ibase::operator!();
    }

    void set_stream(IOStream* stream) NOEXCEPT override
    {
        // Base classes only capture pointers, both to the same stream.
        ibase::set_stream(stream);
        obase::set_stream(stream);
    }

protected:
    // For make_streamer<>.
    byte_flipper() NOEXCEPT : ibase(), obase() {}
    template <class, template <class> class, class, class>
    friend class make_streamer;
};

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()

#endif
