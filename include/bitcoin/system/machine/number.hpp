/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

class BC_API number
{
public:
    /// Exception to number encoding (non-empty zeros, unlimited size).
    /// True if the bytes are true according to script evaluation rules.
    static bool is_stack_true(const data_chunk& bytes) noexcept;

    /// Construct with zero value.
    number() noexcept;

    /// Construct with specified value.
    explicit number(int64_t value) noexcept;

    /// Replace the value derived from a little-endian byte vector.
    bool set_data(const data_chunk& bytes, size_t max_size) noexcept;

    /// Return the value as a byte vector with LSB first ordering.
    data_chunk data() const noexcept;

    /// Return the value as int64. (up to 8 bytes stack data).
    int64_t int64() const noexcept;

private:
    int64_t value_;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
