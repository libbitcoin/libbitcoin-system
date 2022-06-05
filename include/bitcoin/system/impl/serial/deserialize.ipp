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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_DESERIALZIE_IPP
#define LIBBITCOIN_SYSTEM_SERIAL_DESERIALZIE_IPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>

 // Cannot use a data_slice override as it is not deserializable.

namespace libbitcoin {
namespace system {

template <typename Value>
bool deserialize(Value& out, std::istream& input) noexcept
{
    // Consumes an entire whitespace-delimited stream.
    std::istreambuf_iterator<char> begin(input), end;
    std::string text(begin, end);
    return deserialize(out, text);
}

inline bool deserialize(std::string& out, const std::string& text) noexcept
{
    out.assign(text);
    return true;
}

inline bool deserialize(uint8_t& out, const std::string& text) noexcept
{
    uint16_t value;
    if (!deserialize(value, text))
        return false;

    out = narrow_cast<uint8_t>(value);
    return value <= max_uint8;
}

template <size_t Size>
bool deserialize(data_array<Size>& out, const std::string& text) noexcept
{
    return decode_base16(out, text);
}

inline bool deserialize(data_chunk& out, const std::string& text) noexcept
{
    return decode_base16(out, text);
}

template <typename Value, size_t Size>
bool deserialize(std::array<Value, Size>& out, const std::string& text) noexcept
{
    auto result = true;
    const auto deserializer = [&result](const std::string& token) noexcept
    {
        Value value{};
        result &= deserialize(value, token);
        return value;
    };

    const auto tokens = split(text);
    std::transform(tokens.begin(), tokens.end(), out.begin(), deserializer);
    return result;
}

template <typename Value>
bool deserialize(std::vector<Value>& out, const std::string& text) noexcept
{
    auto result = true;
    const auto deserializer = [&result](const std::string& token) noexcept
    {
        Value value{};
        result &= deserialize(value, token);
        return value;
    };

    const auto tokens = split(text);
    out.resize(tokens.size());
    std::transform(tokens.begin(), tokens.end(), out.begin(), deserializer);
    return result;
}

template <typename Value>
bool deserialize(Value& out, const std::string& text) noexcept
{
    // Suppress istringstream may throw inside noexcept.
    // The intended behavior in this case is program abort.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

    // Trimming is useful for type conversion, which otherwise fails.
    // So trimming of string types (pass-thru) is avoided by template override.
    // This can convert garbage to zero, use is_ascii_number for pre-assurance.
    std::istringstream istream(trim_copy(text));
    istream >> out;
    return !istream.fail();

    BC_POP_WARNING()
}

} // namespace system
} // namespace libbitcoin

#endif
