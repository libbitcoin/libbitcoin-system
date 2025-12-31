/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_VERSION_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_VERSION_HPP

#include <memory>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Container for a protocol version tuple with up to four segments.
/// Segments are non-negative integers, padded with zeros for fewer than four.
/// The format is major.minor[.subminor[.patch]] with at least one dot.
class BC_API version
{
public:
    typedef std::shared_ptr<version> ptr;

    DEFAULT_COPY_MOVE_DESTRUCT(version);

    version() NOEXCEPT;

    /// Deserialize from dotted string (throws on invalid format).
    version(const std::string& version_str) THROWS;

    /// Construct from individual segments (2 to 4, pads with zeros).
    version(uint32_t major, uint32_t minor, uint32_t subminor={},
        uint32_t patch={}) NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Access to the internal segments array.
    const std::array<uint32_t, 4>& segments() const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    /// The version is 0.0.0.0.
    bool is_default() const NOEXCEPT;

    /// Serialize to dot string, omitting trailing zero segments (minimum 2).
    std::string to_string() const NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    /// Deserialize from input stream (throws on invalid format).
    friend std::istream& operator>>(std::istream& input, version& argument) THROWS;

    /// Serialize to output stream.
    friend std::ostream& operator<<(std::ostream& output,
        const version& argument) NOEXCEPT;

private:
    // This is not thread safe.
    std::array<uint32_t, 4> segments_;
};

/// Lexicographical comparison.
BC_API bool operator==(const version& left, const version& right) NOEXCEPT;
BC_API bool operator!=(const version& left, const version& right) NOEXCEPT;
BC_API bool operator<(const version& left, const version& right) NOEXCEPT;
BC_API bool operator<=(const version& left, const version& right) NOEXCEPT;
BC_API bool operator>(const version& left, const version& right) NOEXCEPT;
BC_API bool operator>=(const version& left, const version& right) NOEXCEPT;

typedef std::vector<version> versions;

} // namespace config
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::config::version>
{
    size_t operator()(const bc::system::config::version& value) const NOEXCEPT
    {
        return std::hash<std::string>{}(value.to_string());
    }
};
} // namespace std

#endif
