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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_FILTER_IPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_FILTER_IPP

namespace libbitcoin {
namespace system {
namespace unit_test {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

inline bool filter::matches(const std::string& pattern,
    const std::string& name) NOEXCEPT
{
    constexpr auto none = std::string::npos;
    size_t at{};
    size_t position{};
    auto star = none;
    size_t resume{};

    while (position < name.size())
    {
        if (at < pattern.size() && pattern.at(at) == '*')
        {
            star = at++;
            resume = position;
        }
        else if (at < pattern.size() && pattern.at(at) == name.at(position))
        {
            ++at;
            ++position;
        }
        else if (star != none)
        {
            at = add1(star);
            position = ++resume;
        }
        else
        {
            return false;
        }
    }

    while (at < pattern.size() && pattern.at(at) == '*')
        ++at;

    return at == pattern.size();
}

inline filter::filter(const std::vector<std::string>& filters) NOEXCEPT
  : filters_(filters)
{
}

inline bool filter::selects(const path& units) const NOEXCEPT
{
    auto included = false;
    auto inclusions = false;
    for (const auto& rule: filters_)
    {
        if (!rule.empty() && rule.front() == '!')
        {
            if (matches_filter(rule.substr(one), units))
                return false;
        }
        else
        {
            inclusions = true;
            included = included || matches_filter(rule, units);
        }
    }

    return included || !inclusions;
}

// private
// ----------------------------------------------------------------------------

inline std::vector<std::string> filter::split(const std::string& text,
    char delimiter) NOEXCEPT
{
    std::vector<std::string> out{};
    size_t start{};
    while (true)
    {
        const auto stop = text.find(delimiter, start);
        if (stop == std::string::npos)
        {
            out.push_back(text.substr(start));
            return out;
        }

        out.push_back(text.substr(start, stop - start));
        start = add1(stop);
    }
}

inline bool filter::matches_filter(const std::string& rule,
    const path& units) NOEXCEPT
{
    const auto levels = split(rule, '/');
    if (levels.size() > units.size())
        return false;

    for (size_t level{}; level < levels.size(); ++level)
    {
        auto any = false;
        for (const auto& pattern: split(levels.at(level), ','))
            any = any || matches(pattern, units.at(level));

        if (!any)
            return false;
    }

    return true;
}

BC_POP_WARNING()

} // namespace unit_test
} // namespace system
} // namespace libbitcoin

#endif
