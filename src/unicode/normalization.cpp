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
#include <bitcoin/system/unicode/normalization.hpp>

#include <algorithm>
#include <mutex>
#ifdef HAVE_MSC
#else
    #include <mutex>
#endif
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/ascii.hpp>
#include <bitcoin/system/unicode/code_points.hpp>
#include <bitcoin/system/unicode/conversion.hpp>

namespace libbitcoin {
namespace system {

// Avoid codecvt as it is deprecated in C++17.
using namespace boost::locale;

// Local helpers.
// ----------------------------------------------------------------------------

constexpr bool is_contained(char32_t value,
    const char32_interval& interval) NOEXCEPT
{
    return interval.first <= value && value <= interval.second;
}

#ifdef HAVE_MSC

// Workarounds for lack of Windows ICU support in boost-locale packages.
// The ICU library was first added to Windows 10 in [10.0.15063].
// docs.microsoft.com/en-us/windows/win32/intl/international-components-for-unicode--icu-
// Windows XP, Windows Server 2003: No longer supported.
// The required header file and DLL are part of the Microsoft Internationalized
// Domain Name(IDN) Mitigation APIs, which are no longer available for download.

static NORM_FORM to_win32_normal_form(norm_type form) NOEXCEPT
{
    switch (form)
    {
        case norm_type::norm_nfkd:
            return NormalizationKD;
        case norm_type::norm_nfkc:
            return NormalizationKC;
        case norm_type::norm_nfd:
            return NormalizationD;

        // NFC is the boost::locale default and this is the full enumeration.
        case norm_type::norm_nfc:
        default:
            return NormalizationC;
    }
}

static bool normal_form(std::string& out, const std::string& in,
    norm_type form) NOEXCEPT
{
#ifndef HAVE_ICU
    return false;
#endif

    const auto wide = to_utf16(in);
    const auto size = wide.size();
    const auto source = wide.c_str();
    const auto norm = to_win32_normal_form(form);

    // Guard cast to int.
    if (is_limited<int>(size))
        return false;

    const auto length = static_cast<int>(size);
    auto result = NormalizeString(norm, source, length, NULL, 0);

    // NormalizeString would overflow buffer (or length is empty).
    if (result <= 0)
        return false;

    auto buffer = std::wstring(result, 0);
    result = NormalizeString(norm, source, length, &buffer.front(), result);

    // Conversion failed.
    if (result <= 0)
        return false;

    out = to_utf8(buffer.substr(0, result));
    return true;
}

bool to_lower(std::string& out, const std::string& in) NOEXCEPT
{
#ifndef HAVE_ICU
    return false;
#endif

    auto wide = to_utf16(in);
    const auto size = wide.size();

    // Guard against DWORD overflow.
    if (is_limited<DWORD>(size))
        return false;

    // std_vector ensures contiguous bytes.
    const auto length = static_cast<DWORD>(size);

    // CharLowerBuffW ensures conversion in place.
    if (CharLowerBuffW(&wide.front(), length) != length)
        return false;

    out = to_utf8(wide);
    return true;
}

bool to_upper(std::string& out, const std::string& in) NOEXCEPT
{
#ifndef HAVE_ICU
    return false;
#endif

    auto wide = to_utf16(in);
    const auto size = wide.size();

    // Guard against DWORD overflow.
    if (is_limited<DWORD>(size))
        return false;

    // std_vector ensures contiguous bytes.
    const auto length = static_cast<DWORD>(size);

    // CharUpperBuffW ensures conversion in place.
    if (CharUpperBuffW(&wide.front(), length) != length)
        return false;

    out = to_utf8(wide);
    return true;
}

#else // HAVE_MSC

constexpr auto icu_backend_name = "icu";
constexpr auto utf8_locale_name = "en_US.UTF8";

static bool get_backend_manager(localization_backend_manager& out) NOEXCEPT
{
    static std::once_flag mutex;
    static bool initialized;

    // Thread safe, creates global on first call.
    // Returns reference to the global backend manager.
    out = localization_backend_manager::global();

    // Set the static initialization state.
    const auto validate = [&]() NOEXCEPT
    {
        // Not thread safe, use call_once.
        const auto all = out.get_all_backends();

        // Guards backend_manager.select(BC_LOCALE_BACKEND) silent failure.
        initialized = std::find(all.cbegin(), all.cend(),
            icu_backend_name) != all.cend();
    };

    // One time verifier of the localization backend manager.
    std::call_once(mutex, validate);
    return initialized;
}

static bool normal_form(std::string& out, const std::string& in,
    norm_type form) NOEXCEPT
{
#ifndef HAVE_ICU
    return false;
#endif

    localization_backend_manager manager;
    if (!get_backend_manager(manager))
        return false;

    manager.select(icu_backend_name);
    const generator locale(manager);
    out = normalize(in, form, locale(utf8_locale_name));
    return true;
}

bool to_lower(std::string& out, const std::string& in) NOEXCEPT
{
#ifndef HAVE_ICU
    return false;
#endif

    localization_backend_manager manager;
    if (!get_backend_manager(manager))
        return false;

    manager.select(icu_backend_name);
    const generator locale(manager);
    out = boost::locale::to_lower(in, locale(utf8_locale_name));
    return true;
}

bool to_upper(std::string& out, const std::string& in) NOEXCEPT
{
#ifndef HAVE_ICU
    return false;
#endif

    localization_backend_manager manager;
    if (!get_backend_manager(manager))
        return false;

    manager.select(icu_backend_name);
    const generator locale(manager);
    out = boost::locale::to_upper(in, locale(utf8_locale_name));
    return true;
}

#endif // HAVE_MSC

// ICU dependency (ascii supported, otherwise false if HAVE_ICU not defined).
// ----------------------------------------------------------------------------

bool to_lower(std::string& value) NOEXCEPT
{
    if (is_ascii(value))
    {
        value = ascii_to_lower(value);
        return true;
    }

    return to_lower(value, value);
}

bool to_upper(std::string& value) NOEXCEPT
{
    if (is_ascii(value))
    {
        value = ascii_to_upper(value);
        return true;
    }

    return to_upper(value, value);
}

bool to_canonical_composition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, value, norm_type::norm_nfc);
}

bool to_canonical_decomposition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, value, norm_type::norm_nfd);
}

bool to_compatibility_composition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, value, norm_type::norm_nfkc);
}

bool to_compatibility_decomposition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, value, norm_type::norm_nfkd);
}

// No ICU dependency.
// ----------------------------------------------------------------------------

bool is_unicode(char32_t point) NOEXCEPT
{
    return point < 0x0010ffff;
}

bool is_separator(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_separators_count; ++index)
        if (point == char32_separators[index])
            return true;

    return false;
}

bool is_whitespace(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_whitespace_count; ++index)
        if (point == char32_whitespace[index])
            return true;

    return false;
}

bool is_combining(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    // github.com/python/cpython/blob/main/Modules/unicodedata.c
    const auto data1 = unicode_data1[(point >> 7)];
    const auto data2 = unicode_data2[(data1 << 7) + (point & sub1(1 << 7))];
    return !is_zero(combining_index[data2]);
}

bool is_diacritic(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_diacritics_count; ++index)
        if (is_contained(point, char32_diacritics[index]))
            return true;

    return false;
}

bool is_chinese_japanese_or_korean(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_chinese_japanese_korean_count; ++index)
        if (is_contained(point, char32_chinese_japanese_korean[index]))
            return true;

    return false;
}

bool has_whitespace(const std::string& value) NOEXCEPT
{
    if (value.empty())
        return false;

    const auto points = to_utf32(value);
    return std::any_of(points.begin(), points.end(), [](char32_t character)
    {
        return is_whitespace(character);
    });
}

std::string to_non_combining_form(const std::string& value) NOEXCEPT
{
    if (value.empty())
        return value;

    // utf32 ensures each word is a single unicode character.
    auto points = to_utf32(value);
    std::erase_if(points, is_combining);
    return to_utf8(points);
}

std::string to_non_diacritic_form(const std::string& value) NOEXCEPT
{
    if (value.empty())
        return value;

    // utf32 ensures each word is a single unicode character.
    auto points = to_utf32(value);
    std::erase_if(points, is_diacritic);
    return to_utf8(points);
}

// Compress ascii whitespace and remove ascii spaces between cjk characters.
std::string to_compressed_form(const std::string& value) NOEXCEPT
{
    // Compress ascii whitespace to a single 0x20 between each utf32 token.
    const auto normalized = system::join(system::split(value));

    // utf32 ensures each word is a single unicode character.
    const auto points = to_utf32(normalized);

    // A character cannot be between two others if there aren't at least three.
    if (points.size() < 3u)
        return normalized;

    // Copy the first character to output.
    std::u32string compressed{ points.front() };

    // Remove a single ascii whitespace between CJK characters.
    // Front and back cannot be between two characters, so skip them.
    for (size_t point = 1; point < sub1(points.size()); point++)
    {
        if (!(is_ascii_whitespace(points[point]) &&
            is_chinese_japanese_or_korean(points[sub1(point)]) &&
            is_chinese_japanese_or_korean(points[add1(point)])))
        {
            compressed += points[point];
        }
    }

    // Copy the last character to output.
    compressed += points.back();
    return to_utf8(compressed);
}

} // namespace system
} // namespace libbitcoin
