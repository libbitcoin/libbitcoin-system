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
#include <bitcoin/system/config/setting.hpp>

#include <locale>
#include <sstream>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

bool is_configured(const variables_map& variables,
    const std::string& name) NOEXCEPT
{
    return !variables[name].empty() && !variables[name].defaulted();
}

// Comma thousands separators in groups of three, independent of locale.
class comma_grouping final
  : public std::numpunct<char>
{
protected:
    char do_thousands_sep() const override
    {
        return ',';
    }

    std::string do_grouping() const override
    {
        return "\3";
    }
};

template <typename Integer>
static std::string parse_grouped(const std::string& text) NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    static const std::locale grouped
    {
        std::locale::classic(), new comma_grouping{}
    };
    BC_POP_WARNING()

    Integer value{};
    std::istringstream stream{ text };
    stream.imbue(grouped);
    stream >> std::noskipws >> value;
    return (stream.fail() || !stream.eof()) ? text : std::to_string(value);
}

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
std::string ungroup(const std::string& text) NOEXCEPT
{
    return text.starts_with('-') ? parse_grouped<int64_t>(text) :
        parse_grouped<uint64_t>(text);
}
BC_POP_WARNING()

} // namespace config
} // namespace system
} // namespace libbitcoin
