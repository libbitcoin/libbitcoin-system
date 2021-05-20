/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_BIT_WRITER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_BIT_WRITER_HPP

#include <cstdint>
#include <bitcoin/system/iostream/writer.hpp>

// The bit writer operates from high to low order bits.
// It reads and writes high bits first both in the operand and the buffer.
// The bit order is independent of the byte order (big vs. little endian).
// We refer to this as a "high endian" bit stream writer.
// Both big and little byte orderings are supported by the bit writer.

namespace libbitcoin {
namespace system {

/// Writer interface.
class BC_API bit_writer
  : public writer
{
public:
    /// Write bit.
    virtual void write_bit(bool value) = 0;
    virtual void write_bits(uint64_t value, uint8_t bits) = 0;
};

} // namespace system
} // namespace libbitcoin

#endif
