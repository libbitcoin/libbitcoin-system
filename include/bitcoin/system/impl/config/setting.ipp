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
#ifndef LIBBITCOIN_SYSTEM_IMPL_CONFIG_SETTING_IPP
#define LIBBITCOIN_SYSTEM_IMPL_CONFIG_SETTING_IPP

#include <sstream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp>

namespace libbitcoin {
namespace system {
namespace config {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Path insertion is quoted, and wide on win32.
inline std::string serialize(const std::filesystem::path& value) NOEXCEPT
{
    return from_path(value);
}

template <typename Type>
std::string serialize(const Type& value) NOEXCEPT
{
    std::ostringstream stream{};
    stream << std::boolalpha << value;
    return stream.str();
}

template <typename Type>
setting_value<Type>::setting_value(Type* store) THROWS
  : boost::program_options::typed_value<Type>(store), store_(store)
{
}

template <typename Type>
string_list setting_value<Type>::values() const NOEXCEPT
{
    string_list out{};

    if constexpr (is_std_vector<Type>)
    {
        out.reserve(store_->size());

        for (const auto& element: *store_)
            out.push_back(serialize(element));
    }
    else
    {
        out.push_back(serialize(*store_));
    }

    return out;
}

template <typename Type>
setting_value<Type>* setting(Type* store) THROWS
{
    return new setting_value<Type>(store);
}

BC_POP_WARNING()

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
