/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/config/version.hpp>

#include <algorithm>
#include <sstream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace config {

// Contructors.
// ----------------------------------------------------------------------------

// Default version is 0.0.0.0 and serializes as "0.0".
version::version() NOEXCEPT
  : segments_{}
{
}

version::version(const std::string& version) THROWS
  : segments_{}
{
    std::istringstream(version) >> (*this);
}

version::version(uint32_t major, uint32_t minor, uint32_t subminor,
    uint32_t patch) NOEXCEPT
  : segments_{ major, minor, subminor, patch }
{
}

// Properties.
// ----------------------------------------------------------------------------

const std::array<uint32_t, 4>& version::segments() const NOEXCEPT
{
    return segments_;
}

// Methods.
// ----------------------------------------------------------------------------

bool version::is_default() const NOEXCEPT
{
    constexpr  std::array<uint32_t, 4> default_{};
    return segments_ == default_;
}

std::string version::to_string() const NOEXCEPT
{
    std::ostringstream value{};
    value << (*this);
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return value.str();
    BC_POP_WARNING()
}

// Operators.
// ----------------------------------------------------------------------------

bool operator==(const version& left, const version& right) NOEXCEPT
{
    return left.segments() == right.segments();
}

bool operator!=(const version& left, const version& right) NOEXCEPT
{
    return !(left == right);
}

bool operator<(const version& left, const version& right) NOEXCEPT
{
    return std::lexicographical_compare(
        left.segments().begin(), left.segments().end(),
        right.segments().begin(), right.segments().end());
}

bool operator<=(const version& left, const version& right) NOEXCEPT
{
    return left < right || left == right;
}

bool operator>(const version& left, const version& right) NOEXCEPT
{
    return right < left;
}

bool operator>=(const version& left, const version& right) NOEXCEPT
{
    return right <= left;
}

std::istream& operator>>(std::istream& input,
    version& argument) THROWS
{
    std::string value;
    input >> value;

    using namespace system;
    const auto tokens = system::split(value, ".");
    const auto count = tokens.size();
    if (count < 2u || count > 4u)
        throw istream_exception(value);

    argument = {};
    for (size_t index{}; index < count; ++index)
        if (!deserialize(argument.segments_.at(index), tokens.at(index)))
            throw istream_exception(value);

    return input;
}

std::ostream& operator<<(std::ostream& output,
    const version& argument) NOEXCEPT
{
    size_t last{ 3 };
    while (last > 1u && is_zero(argument.segments_.at(last)))
        --last;

    for (size_t index{}; index <= last; ++index)
    {
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        if (!is_zero(index)) output << '.';
        output << argument.segments_.at(index);
        BC_POP_WARNING()
    }

    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
