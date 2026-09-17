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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_PATH_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_PATH_HPP

#include <filesystem>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for filesystem paths.
/// Text is utf8 encoded, may be empty, and may contain spaces.
class BC_API path final
{
public:
    typedef std::filesystem::path type;

    DEFAULT_COPY_MOVE_DESTRUCT(path);

    path() NOEXCEPT;
    path(type value) NOEXCEPT;

    /// Deserialize from utf8 string.
    path(const std::string& value) NOEXCEPT;
    path(const char* value) NOEXCEPT;

    operator const type&() const NOEXCEPT;

    bool operator==(const path& other) const NOEXCEPT;
    bool operator!=(const path& other) const NOEXCEPT;

    /// True if the path is unset.
    bool empty() const NOEXCEPT;

    /// The path as a utf8 encoded string.
    std::string to_string() const NOEXCEPT;

    /// Deserialize from input stream (reads the full line).
    friend std::istream& operator>>(std::istream& input, path& argument)
        NOEXCEPT;

    /// Serialize to output stream.
    friend std::ostream& operator<<(std::ostream& output,
        const path& argument) NOEXCEPT;

private:
    type value_;
};

/// Append a relative path, as the underlying type.
BC_API std::filesystem::path operator/(const path& left,
    const std::filesystem::path& right) NOEXCEPT;

/// Assign the token as the path, bypassing stream extraction, which cannot
/// obtain an empty token and terminates a token at its first space.
BC_API void validate(boost::any& value, const string_list& tokens, path*,
    int) THROWS;

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
