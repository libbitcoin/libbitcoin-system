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
#ifndef LIBBITCOIN_SYSTEM_DATA_BINARY_HPP
#define LIBBITCOIN_SYSTEM_DATA_BINARY_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/data/uintx.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// Constant class for bit vector serialization and comparison.
class BC_API binary
{
public:
    /// uintx is limited to unsigned.
    typedef uint32_t size_type;

    /// True if all characters are '0' or '1'.
    static bool is_base2(const std::string& text) noexcept;

    /// Constructors.
    binary() noexcept;
    binary(binary&& other) noexcept;
    binary(const binary& other) noexcept;
    binary(const std::string& bits) noexcept;
    binary(size_type size, const data_slice& data) noexcept;

    /// Methods.
    std::string encoded() const noexcept;
    data_chunk data() const noexcept;
    size_type bytes() const noexcept;
    size_type bits() const noexcept;

    /// Operators.
    bool operator[](size_type index) const noexcept;
    bool operator<(const binary& other) const noexcept;
    binary& operator=(binary&& other) noexcept;
    binary& operator=(const binary& other) noexcept;

private:
    static binary from_data(size_type size, const data_slice& data) noexcept;
    static binary from_string(const std::string bits) noexcept;
    binary(size_type size, const uintx& number) noexcept;

    uintx bits_;
    size_type size_;
};

bool operator==(const binary& left, const binary& right) noexcept;
bool operator!=(const binary& left, const binary& right) noexcept;
std::istream& operator>>(std::istream& in, binary& to) noexcept;
std::ostream& operator<<(std::ostream& out, const binary& of) noexcept;

} // namespace system
} // namespace libbitcoin

namespace std
{

template<>
struct hash<bc::system::binary>
{
    size_t operator()(const bc::system::binary& value) const
    {
        return std::hash<std::string>()(value.encoded());
    }
};

} // namespace std

#endif
