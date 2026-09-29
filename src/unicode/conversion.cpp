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
#include <bitcoin/system/unicode/conversion.hpp>

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Surrogates and values above the unicode range are not code points.
constexpr bool is_code_point(char32_t point) NOEXCEPT
{
    return point <= 0x0010ffff && (point < 0x0000d800 || point > 0x0000dfff);
}

// Decode one code point from UTF8 (char), UTF16 or UTF32 (by char size).
// False if the sequence is truncated, overlong, or not a code point.
template <typename Char>
static bool decode(char32_t& point, const std::basic_string<Char>& in,
    size_t& offset) NOEXCEPT
{
    if constexpr (sizeof(Char) == sizeof(uint8_t))
    {
        const auto lead = static_cast<uint8_t>(in.at(offset++));
        if (lead < 0x80)
        {
            point = lead;
            return true;
        }

        size_t trail{};
        char32_t minimum{};
        if ((lead & 0xe0) == 0xc0)
        {
            trail = 1;
            minimum = 0x00000080;
            point = lead & 0x1f;
        }
        else if ((lead & 0xf0) == 0xe0)
        {
            trail = 2;
            minimum = 0x00000800;
            point = lead & 0x0f;
        }
        else if ((lead & 0xf8) == 0xf0)
        {
            trail = 3;
            minimum = 0x00010000;
            point = lead & 0x07;
        }
        else
        {
            return false;
        }

        if (in.size() - offset < trail)
            return false;

        for (; is_nonzero(trail); --trail)
        {
            const auto next = static_cast<uint8_t>(in.at(offset++));
            if ((next & 0xc0) != 0x80)
                return false;

            point = (point << 6) | (next & 0x3f);
        }

        return point >= minimum && is_code_point(point);
    }
    else if constexpr (sizeof(Char) == sizeof(uint16_t))
    {
        const char32_t high = static_cast<uint16_t>(in.at(offset++));
        if (high < 0xd800 || high > 0xdfff)
        {
            point = high;
            return true;
        }

        if (high > 0xdbff || offset == in.size())
            return false;

        const char32_t low = static_cast<uint16_t>(in.at(offset++));
        if (low < 0xdc00 || low > 0xdfff)
            return false;

        point = 0x00010000 + ((high - 0xd800) << 10) + (low - 0xdc00);
        return true;
    }
    else
    {
        point = static_cast<char32_t>(in.at(offset++));
        return is_code_point(point);
    }
}

// Encode one code point as UTF8 (char), UTF16 or UTF32 (by char size).
template <typename Char>
static void encode(std::basic_string<Char>& out, char32_t point) NOEXCEPT
{
    const auto put = [&](char32_t value) NOEXCEPT
    {
        out.push_back(static_cast<Char>(value));
    };

    if constexpr (sizeof(Char) == sizeof(uint8_t))
    {
        if (point < 0x00000080)
        {
            put(point);
        }
        else if (point < 0x00000800)
        {
            put(0xc0 | (point >> 6));
            put(0x80 | (point & 0x3f));
        }
        else if (point < 0x00010000)
        {
            put(0xe0 | (point >> 12));
            put(0x80 | ((point >> 6) & 0x3f));
            put(0x80 | (point & 0x3f));
        }
        else
        {
            put(0xf0 | (point >> 18));
            put(0x80 | ((point >> 12) & 0x3f));
            put(0x80 | ((point >> 6) & 0x3f));
            put(0x80 | (point & 0x3f));
        }
    }
    else if constexpr (sizeof(Char) == sizeof(uint16_t))
    {
        if (point < 0x00010000)
        {
            put(point);
        }
        else
        {
            const auto value = point - 0x00010000;
            put(0xd800 + (value >> 10));
            put(0xdc00 + (value & 0x03ff));
        }
    }
    else
    {
        put(point);
    }
}

// Empty if any sequence is invalid (no replacement or skipping).
template <typename CharOut, typename CharIn>
static std::basic_string<CharOut> to_utf(
    const std::basic_string<CharIn>& in) NOEXCEPT
{
    std::basic_string<CharOut> out{};
    out.reserve(in.size());

    char32_t point{};
    for (size_t offset{}; offset < in.size();)
    {
        if (!decode(point, in, offset))
            return {};

        encode(out, point);
    }

    return out;
}

template <typename CharOut, typename CharIn>
static std::vector<std::basic_string<CharOut>> to_utf(
    const std::vector<std::basic_string<CharIn>>& in) NOEXCEPT
{
    std::vector<std::basic_string<CharOut>> out(in.size());
    std::transform(in.begin(), in.end(), out.begin(),
        [](const std::basic_string<CharIn>& word) NOEXCEPT
        {
            return to_utf<CharOut>(word);
        });

    return out;
}

// char32_t is the only 1:1 char encoding.
std::string to_utf8(char32_t point) NOEXCEPT
{
    return to_utf8(std::u32string{ point });
}

std::string to_utf8(const std::wstring& text) NOEXCEPT
{
    return to_utf<char>(text);
}

std::string to_utf8(const std::u32string& text) NOEXCEPT
{
    return to_utf<char>(text);
}

std::wstring to_utf16(const std::string& text) NOEXCEPT
{
    return to_utf<wchar_t>(text);
}

std::wstring to_utf16(const std::u32string& text) NOEXCEPT
{
    return to_utf<wchar_t>(text);
}

std::u32string to_utf32(const std::string& text) NOEXCEPT
{
    return to_utf<char32_t>(text);
}

std::u32string to_utf32(const std::wstring& text) NOEXCEPT
{
    return to_utf<char32_t>(text);
}

string_list to_utf8(const wstring_list& text) NOEXCEPT
{
    return to_utf<char>(text);
}

string_list to_utf8(const u32string_list& text) NOEXCEPT
{
    return to_utf<char>(text);
}

wstring_list to_utf16(const string_list& text) NOEXCEPT
{
    return to_utf<wchar_t>(text);
}

wstring_list to_utf16(const u32string_list& text) NOEXCEPT
{
    return to_utf<wchar_t>(text);
}

u32string_list to_utf32(const string_list& text) NOEXCEPT
{
    return to_utf<char32_t>(text);
}

u32string_list to_utf32(const wstring_list& text) NOEXCEPT
{
    return to_utf<char32_t>(text);
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
