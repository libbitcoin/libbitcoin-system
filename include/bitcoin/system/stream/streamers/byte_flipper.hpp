/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

#include <iostream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/streamers/byte_reader.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>
#include <bitcoin/system/stream/streamers/interfaces/byteflipper.hpp>

// Suppress multiple inheritance warnings.
// The only multiple inheritance conflict is destructors and bool/!.
BC_PUSH_WARNING(4250)

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
    byte_flipper(IOStream& stream) noexcept
      : byte_reader<IOStream>(stream), byte_writer<IOStream>(stream)
    {
        // Base constructions only capture references.
        // There are two references to the iostream:
        // byte_reader (istream&)
        // byte_writer (ostream&)
    }

    /// Defaults.
    byte_flipper(byte_flipper&&) = default;
    byte_flipper(const byte_flipper&) = default;
    byte_flipper& operator=(byte_flipper&&) = default;
    byte_flipper& operator=(const byte_flipper&) = default;

    // Two base destructor calls order is unimportant (only writes flush).
    ~byte_flipper() override = default;

    operator bool() const noexcept override
    {
        // Rely on reader implementation, both are trivial and identical.
        return byte_reader<IOStream>::operator bool();
    }

    bool operator!() const noexcept override
    {
        // Rely on reader implementation, both are trivial and identical.
        return byte_reader<IOStream>::operator!();
    }
};

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()

#endif
