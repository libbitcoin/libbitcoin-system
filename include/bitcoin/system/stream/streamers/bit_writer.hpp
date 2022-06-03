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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BIT_WRITER_HPP
#define LIBBITCOIN_SYSTEM_STREAM_STREAMERS_BIT_WRITER_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/streamers/byte_writer.hpp>
#include <bitcoin/system/stream/streamers/interfaces/bitwriter.hpp>

// Suppress multiple inheritance warnings.
// The inheritance is virtual, so not actually multiple.
// But the boost type constraint 'is_virtual_base_of' triggers the warning.
BC_PUSH_WARNING(4250)

namespace libbitcoin {
namespace system {

/// A bit writer that accepts an ostream.
template <typename OStream = std::ostream>
class bit_writer
  : public byte_writer<OStream>,
    public virtual bitwriter
{
public:
    /// Constructors.
    bit_writer(OStream& sink) noexcept;

    /// Copy/move/destruct.
    bit_writer(bit_writer&&);
    bit_writer(const bit_writer&);
    bit_writer& operator=(bit_writer&&);
    bit_writer& operator=(const bit_writer&);
    ~bit_writer() noexcept override;

    /// Write one bit (high to low).
    void write_bit(bool value) noexcept override;

    /// Write size bits from an integer (high to low).
    void write_bits(uint64_t value, size_t bits) noexcept override;

protected:
    void do_write_bytes(const uint8_t* data, size_t size) noexcept override;
    void do_flush() noexcept override;

private:
    void unload() noexcept;
    void flusher() noexcept;
    uint8_t shift() const noexcept;

    uint8_t byte_;
    uint8_t offset_;
};

} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()

#include <bitcoin/system/impl/stream/streamers/bit_writer.ipp>

#endif
