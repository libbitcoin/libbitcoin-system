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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_SERIALIZE_IPP
#define LIBBITCOIN_SYSTEM_SERIAL_SERIALIZE_IPP

#include <charconv>
#include <cmath>
#include <sstream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/serial/deserialize.hpp>

namespace libbitcoin {
namespace system {

// Cannot use a data_slice override for data_array/data_chunk because it would
// pick up string and serialize as bytes vs. chars.
    
template <typename Value>
void serialize(std::ostream& output, const Value& value,
    const std::string& fallback) NOEXCEPT
{
    output << serialize(value, fallback);
}

inline std::string serialize(uint8_t value,
    const std::string& fallback) NOEXCEPT
{
    return serialize(static_cast<uint16_t>(value), fallback);
}

inline std::string serialize(char value, const std::string&) NOEXCEPT
{
    return { value };
}

template <size_t Size>
std::string serialize(const data_array<Size>& value,
    const std::string&) NOEXCEPT
{
    return encode_base16(value);
}

inline std::string serialize(const data_chunk& value,
    const std::string&) NOEXCEPT
{
    return encode_base16(value);
}

template <typename Value, size_t Size>
std::string serialize(const std_array<Value, Size>& values,
    const std::string& fallback) NOEXCEPT
{
    string_list tokens(values.size());
    const auto serializer = [&fallback](const Value& value) NOEXCEPT
    {
        return serialize(value, fallback);
    };

    std::transform(values.cbegin(), values.cend(), tokens.begin(), serializer);
    return join(tokens);
}

template <typename Value>
std::string serialize(const std_vector<Value>& values,
    const std::string& fallback) NOEXCEPT
{
    string_list tokens(values.size());
    const auto serializer = [&fallback](const Value& value) NOEXCEPT
    {
        return serialize(value, fallback);
    };

    std::transform(values.cbegin(), values.cend(), tokens.begin(), serializer);
    return join(tokens);
}

template <typename Integer, if_integral<Integer>>
std::string serialize(Integer value, const std::string&) NOEXCEPT
{
    return std::to_string(value);
}

template <typename Value, if_non_integral<Value>>
std::string serialize(const Value& value, const std::string& fallback) NOEXCEPT
{
    try
    {
        std::ostringstream ostream{};
        ostream << value;
        const auto token = ostream.str();
        return token.empty() ? fallback : token;
    }
    catch (const std::exception&)
    {
        return {};
    }
}

template <typename Float, if_floating_point<Float>>
std::string serialize_fixed(Float value) NOEXCEPT
{
    if (!std::isfinite(value))
        return serialize(value);

    // Unspecified precision is the shortest text that converts to the value.
    using limits = std::numeric_limits<Float>;
    constexpr auto size = to_unsigned(add1(limits::max_exponent10)) +
        fixed_decimals<Float> + two;
    std_array<char, size> buffer{};
    const auto first = buffer.data();
    const auto last = std::next(first, size);
    const auto [end, ec] = std::to_chars(first, last, value,
        std::chars_format::fixed);
    if (ec != std::errc{})
        return {};

    std::string token{ first, end };
    if (token.find('.') == std::string::npos)
        token += ".0";

    return token;
}

} // namespace system
} // namespace libbitcoin

#endif
