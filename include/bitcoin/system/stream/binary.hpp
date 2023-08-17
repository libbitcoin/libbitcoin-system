/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_BINARY_HPP
#define LIBBITCOIN_SYSTEM_STREAM_BINARY_HPP

#include <functional>
#include <iostream>
#include <string>
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Constant class for bit vector serialization and comparison.
class BC_API binary
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(binary);

    /// True if all characters are '0' or '1'.
    static bool is_base2(const std::string& text) NOEXCEPT;

    /// Constructors.
    binary() NOEXCEPT;
    binary(const std::string& bits) NOEXCEPT;
    binary(size_t bits, const data_slice& data) NOEXCEPT;

    /// Methods.
    std::string encoded() const NOEXCEPT;
    const data_chunk& data() const NOEXCEPT;
    size_t bytes() const NOEXCEPT;
    size_t bits() const NOEXCEPT;

    /// Operators.
    operator const data_chunk&() const NOEXCEPT;
    bool operator[](size_t index) const NOEXCEPT;
    bool operator<(const binary& other) const NOEXCEPT;

private:
    binary(data_chunk&& bytes, size_t bits) NOEXCEPT;
    static binary from_string(const std::string bits) NOEXCEPT;
    static binary from_data(size_t size, data_chunk&& bytes) NOEXCEPT;

    size_t bits_;
    data_chunk bytes_;
};

bool operator==(const binary& left, const binary& right) NOEXCEPT;
bool operator!=(const binary& left, const binary& right) NOEXCEPT;
std::istream& operator>>(std::istream& in, binary& to) NOEXCEPT;
std::ostream& operator<<(std::ostream& out, const binary& of) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

namespace std
{

template<>
struct hash<bc::system::binary>
{
    size_t operator()(const bc::system::binary& value) const NOEXCEPT
    {
        return std::hash<std::string>()(value.encoded());
    }
};

} // namespace std

#endif
