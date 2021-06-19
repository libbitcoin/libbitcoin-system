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

#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_DATA_BIT_READER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_DATA_BIT_READER_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/iostream/data/byte_reader.hpp>

namespace libbitcoin {
namespace system {

/// A bit reader that accepts an istream.
class BC_API bit_reader
  : public byte_reader
{
public:
    /// Constructors.
    bit_reader(std::istream& source);
    virtual ~bit_reader();

    /// Read one bit (high to low).
    virtual bool read_bit();

    /// Read size bits into an integer (high to low).
    virtual uint64_t read_bits(uint8_t bits);

protected:
    virtual uint8_t do_peek() override;
    virtual uint8_t do_read() override;
    virtual void do_read(uint8_t* buffer, size_t size) override;
    virtual bool get_valid() const override;
    virtual bool get_exhausted() const override;
    virtual void set_invalid() override;

private:
    void align();
    bool is_aligned() const;
    uint8_t shift() const;

    uint8_t buffer_;
    uint8_t offset_;
};

} // namespace system
} // namespace libbitcoin

#endif
