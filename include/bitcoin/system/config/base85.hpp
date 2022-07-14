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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_BASE85_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_BASE85_HPP

#include <iostream>
#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for base58 sodium keys.
/// Base85 requires four byte alignment, sodium keys are 32 bytes.
class BC_API base85
{
public:
    /// Defaults.
    base85(base85&&) = default;
    base85(const base85&) = default;
    base85& operator=(base85&&) = default;
    base85& operator=(const base85&) = default;
    ~base85() = default;

    /// Constructors.
    base85() NOEXCEPT;
    base85(data_chunk&& value) NOEXCEPT;
    base85(const data_chunk& value) NOEXCEPT;
    base85(const std::string& base85) THROWS;

    /// Operators.

    /// True if the data size is evenly divisible by 4.
    operator bool() const NOEXCEPT;

    operator const data_chunk&() const NOEXCEPT;

    /// The key as a base85 encoded (z85) string.
    std::string to_string() const;

    friend std::istream& operator>>(std::istream& stream,
        base85& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& stream,
        const base85& argument);

private:
    data_chunk value_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
