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
#ifndef LIBBITCOIN_SYSTEM_STREAM_FLIPPERS_BYTE_FLIPPER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_FLIPPERS_BYTE_FLIPPER_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/stream/interfaces/byteflipper.hpp>
#include <bitcoin/system/stream/readers/byte_reader.hpp>
#include <bitcoin/system/stream/writers/byte_writer.hpp>

namespace libbitcoin {
namespace system {
    
// Suppress multiple inheritance warnings.
#pragma warning(push)
#pragma warning(disable : 4250)

/// A byte reader/writer that accepts an iostream.
template <typename IOStream = std::iostream>
class byte_flipper
  : public byte_reader<IOStream>,
    public byte_writer<IOStream>,
    public virtual byteflipper
{
public:
    byte_flipper(IOStream& stream) noexcept
      : byte_reader(stream), byte_writer(stream)
    {
        // Base constructions only capture references.
        // There are two references to the iostream:
        // byte_reader (istream&)
        // byte_writer (ostream&)
    }

    ~byte_flipper() noexcept override
    {
        // Flush writes before reads (only writes matter).
        byte_writer::~byte_writer();
        byte_reader::~byte_reader();
    }
};

#pragma warning(pop)

} // namespace system
} // namespace libbitcoin

#endif
