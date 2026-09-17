/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/config/path.hpp>

#include <filesystem>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp>

namespace libbitcoin {
namespace system {
namespace config {

path::path() NOEXCEPT
  : value_{}
{
}

path::path(type value) NOEXCEPT
  : value_(std::move(value))
{
}

path::path(const std::string& value) NOEXCEPT
  : value_(to_path(value))
{
}

path::path(const char* value) NOEXCEPT
  : value_(to_path(value))
{
}

path::operator const path::type&() const NOEXCEPT
{
    return value_;
}

bool path::operator==(const path& other) const NOEXCEPT
{
    return value_ == other.value_;
}

bool path::operator!=(const path& other) const NOEXCEPT
{
    return !(*this == other);
}

bool path::empty() const NOEXCEPT
{
    return value_.empty();
}

std::string path::to_string() const NOEXCEPT
{
    return from_path(value_);
}

std::filesystem::path operator/(const path& left,
    const std::filesystem::path& right) NOEXCEPT
{
    return static_cast<const path::type&>(left) / right;
}

std::istream& operator>>(std::istream& input, path& argument) NOEXCEPT
{
    std::string value{};
    std::getline(input, value);
    argument.value_ = to_path(value);
    return input;
}

std::ostream& operator<<(std::ostream& output, const path& argument) NOEXCEPT
{
    output << from_path(argument.value_);
    return output;
}

void validate(boost::any& value, const string_list& tokens, path*,
    int) THROWS
{
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(value);

    if (tokens.size() > one)
        throw validation_exception
        {
            validation_exception::multiple_values_not_allowed
        };

    value = boost::any{ path{ tokens.empty() ? std::string{} :
        tokens.front() } };
}

} // namespace config
} // namespace system
} // namespace libbitcoin
