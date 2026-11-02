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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_FILTER_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_FILTER_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace unit_test {

/// Selects test units by run_test filters. A filter is levels joined by '/',
/// each a ','-separated list of patterns, and selects every unit within a
/// matching path. A '!' prefixed filter excludes instead.
class filter
{
public:
    using path = std::vector<std::string>;

    /// True if the name matches the pattern ('*' matches any sequence).
    static bool matches(const std::string& pattern,
        const std::string& name) NOEXCEPT;

    /// No filters selects every unit.
    filter(const std::vector<std::string>& filters) NOEXCEPT;

    /// True if selected by any filter (or there are none) and excluded by
    /// no '!' filter, where units is the path of suites then case.
    bool selects(const path& units) const NOEXCEPT;

private:
    static std::vector<std::string> split(const std::string& text,
        char delimiter) NOEXCEPT;
    static bool matches_filter(const std::string& rule,
        const path& units) NOEXCEPT;

    const std::vector<std::string> filters_;
};

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/unit_test/filter.ipp>

#endif
