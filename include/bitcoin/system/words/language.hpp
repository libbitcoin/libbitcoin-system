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
#ifndef LIBBITCOIN_SYSTEM_WORDS_LANGUAGE_HPP
#define LIBBITCOIN_SYSTEM_WORDS_LANGUAGE_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

typedef enum class language
{
    en,
    es,
    it,
    fr,
    cs,
    pt,
    ja,
    ko,
    zh_Hans,
    zh_Hant,
    none
} language;

} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::language>
{
    size_t operator()(const bc::system::language& value) const NOEXCEPT
    {
        return std::hash<uint8_t>{}(static_cast<uint8_t>(value));
    }
};
} // namespace std

#endif
