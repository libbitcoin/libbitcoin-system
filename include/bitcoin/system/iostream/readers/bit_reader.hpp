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

#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_READERS_BIT_READER_HPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_READERS_BIT_READER_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/iostream/interfaces/bitreader.hpp>
#include <bitcoin/system/iostream/readers/byte_reader.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

// Suppress multiple inheritance warnings.
// The inheritance is virtual, so not actually multiple.
// But the boost type constraint 'is_virtual_base_of' triggers the warning.
#pragma warning(push)
#pragma warning(disable : 4250)

/// A bit reader that accepts an istream.
template <typename IStream = std::istream>
class bit_reader
  : public byte_reader<IStream>,
    public virtual bitreader
{
public:
    /// Constructors.
    bit_reader(IStream& source) noexcept;
    ~bit_reader() noexcept override;

    /// Read one bit (high to low).
    virtual bool read_bit() noexcept;

    /// Read size bits into an integer (high to low).
    ////template <typename Integer, if_integer<Integer> = true>
    virtual uint64_t read_bits(size_t bits) noexcept;

    /// Advance the iterator.
    virtual void skip_bit(size_t bits=one) noexcept;

protected:
    uint8_t do_peek() noexcept override;
    uint8_t do_read() noexcept override;
    void do_read(uint8_t* buffer, size_t size) noexcept override;
    void do_skip(size_t size) noexcept override;
    bool get_exhausted() const noexcept override;
    bool get_valid() const noexcept override;
    void set_invalid() noexcept override;

private:
    void align() noexcept;
    bool is_aligned() const noexcept;
    uint8_t shift() const noexcept;

    uint8_t buffer_;
    uint8_t offset_;
};

#pragma warning(pop)

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/iostream/readers/bit_reader.ipp>

#endif
