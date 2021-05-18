/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <cstddef>
#include <string>
#ifdef _MSC_VER
    #include <limits>
    #include <windows.h>
#else
    #include <mutex>
#endif
#include <boost/locale.hpp>
#include <bitcoin/system/utility/exceptions.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/unicode/ascii.hpp>
#include <bitcoin/system/unicode/code_points.hpp>
#include <bitcoin/system/unicode/conversion.hpp>

namespace libbitcoin {
namespace system {

// Avoid codecvt as it is deprecated in c++17.
using namespace boost::locale;

// Local helpers.
// ----------------------------------------------------------------------------

#ifdef WITH_ICU

#ifdef _MSC_VER

// Workarounds for lack of Windows ICU support in boost-locale packages.
// The ICU library was first added to Windows 10 in [10.0.15063].
// docs.microsoft.com/en-us/windows/win32/intl/international-components-for-unicode--icu-
// Windows XP, Windows Server 2003: No longer supported.
// The required header file and DLL are part of the Microsoft Internationalized
// Domain Name(IDN) Mitigation APIs, which are no longer available for download.

static NORM_FORM to_win32_normal_form(norm_type form)
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

static std::string normal_form(const std::string& value, norm_type form)
{
    if (value.empty())
        return value;

    const auto wide = to_utf16(value);
    const auto size = wide.size();
    const auto source = wide.c_str();
    const auto norm = to_win32_normal_form(form);

    // Guard cast to int.
    if (size > std::numeric_limits<int>::max())
        return {};

    const auto length = static_cast<int>(size);
    auto result = NormalizeString(norm, source, length, NULL, 0);

    // NormalizeString would overflow buffer (or length is empty).
    if (result <= 0)
        return {};

    auto buffer = std::wstring(result, 0);
    result = NormalizeString(norm, source, length, &buffer.front(), result);

    // Conversion failed.
    if (result <= 0)
        return {};

    return to_utf8(buffer.substr(0, result));
}

std::string to_lower(const std::string& value)
{
    if (value.empty())
        return value;

    auto wide = to_utf16(value);
    const auto size = wide.size();

    // Guard against DWORD overflow.
    if (size > std::numeric_limits<DWORD>::max())
        return {};

    // std::vector ensures contiguous bytes.
    const auto length = static_cast<DWORD>(size);

    // CharLowerBuffW ensures conversion in place.
    if (CharLowerBuffW(&wide.front(), length) != length)
        return {};

    return to_utf8(wide);
}

std::string to_upper(const std::string& value)
{
    if (value.empty())
        return value;

    auto wide = to_utf16(value);
    const auto size = wide.size();

    // Guard against DWORD overflow.
    if (size > std::numeric_limits<DWORD>::max())
        return {};

    // std::vector ensures contiguous bytes.
    const auto length = static_cast<DWORD>(size);

    // CharUpperBuffW ensures conversion in place.
    if (CharUpperBuffW(&wide.front(), length) != length)
        return {};

    return to_utf8(wide);
}

#else // _MSC_VER

// Ensure validate_localization is called only once.
static std::once_flag icu_mutex;

// One time verifier of the localization backend manager.
// Guard against backend_manager.select(BC_LOCALE_BACKEND) silent failure.
static void validate_localization()
{
    // Thread safe.
    // Creates a new global backend manager and returns the onld one.
    // TODO: it would be more efficient to just store this globally.
    const auto backend_manager = localization_backend_manager::global();

    // Not thread safe (call to validate_localization is guarded).
    constexpr auto icu_backend_name = "icu";
    const auto available_backends = backend_manager.get_all_backends();
    const auto iterator = std::find(available_backends.cbegin(),
        available_backends.cend(), icu_backend_name);

    if (iterator == available_backends.cend())
        throw dependency_exception(
            "Unicode normalization test failed, a dependency may be missing.");
}

static std::string normal_form(const std::string& value, norm_type form)
{
    if (value.empty())
        return value;

    std::call_once(icu_mutex, validate_localization);

    // Thread safe.
    constexpr auto icu_backend_name = "icu";
    constexpr auto utf8_locale_name = "en_US.UTF8";
    auto backend_manager = localization_backend_manager::global();
    backend_manager.select(icu_backend_name);
    const generator locale(backend_manager);
    return normalize(value, form, locale(utf8_locale_name));
}

std::string to_lower(const std::string& value)
{
    if (value.empty())
        return value;

    std::call_once(icu_mutex, validate_localization);

    // Thread safe.
    constexpr auto icu_backend_name = "icu";
    constexpr auto utf8_locale_name = "en_US.UTF8";
    auto backend_manager = localization_backend_manager::global();
    backend_manager.select(icu_backend_name);
    const generator locale(backend_manager);
    return boost::locale::to_lower(value, locale(utf8_locale_name));
}

std::string to_upper(const std::string& value)
{
    if (value.empty())
        return value;

    std::call_once(icu_mutex, validate_localization);

    // Thread safe.
    constexpr auto icu_backend_name = "icu";
    constexpr auto utf8_locale_name = "en_US.UTF8";
    auto backend_manager = localization_backend_manager::global();
    backend_manager.select(icu_backend_name);
    const generator locale(backend_manager);
    return boost::locale::to_upper(value, locale(utf8_locale_name));
}

#endif // _MSC_VER

std::string to_canonical_composition(const std::string& value)
{
    return normal_form(value, norm_type::norm_nfc);
}

std::string to_canonical_decomposition(const std::string& value)
{
    return normal_form(value, norm_type::norm_nfd);
}

std::string to_compatibility_composition(const std::string& value)
{
    return normal_form(value, norm_type::norm_nfkc);
}

std::string to_compatibility_demposition(const std::string& value)
{
    return normal_form(value, norm_type::norm_nfkd);
}

#endif // WITH_ICU

inline bool is_unicode(char32_t point)
{
    return point < 0x0010ffff;
}

bool is_separator(char32_t point)
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_separators_count; ++index)
        if (point == char32_separators[index])
            return true;

    return false;
}

bool is_whitespace(char32_t point)
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_whitespace_count; ++index)
        if (point == char32_whitespace[index])
            return true;

    return false;
}

bool is_combining(char32_t point)
{
    if (!is_unicode(point))
        return false;

    // github.com/python/cpython/blob/main/Modules/unicodedata.c
    const auto data1 = unicode_data1[(point >> 7)];
    const auto data2 = unicode_data2[(data1 << 7) + (point & ((1 << 7) - 1))];
    return combining_index[data2] != 0;
}

bool is_diacritic(char32_t point)
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_diacritics_count; ++index)
        if (point >= char32_diacritics[index].first &&
            point <= char32_diacritics[index].second)
            return true;

    return false;
}

bool is_chinese_japanese_or_korean(char32_t point)
{
    if (!is_unicode(point))
        return false;

    for (size_t index = 0; index < char32_chinese_japanese_korean_count; ++index)
        if (point >= char32_chinese_japanese_korean[index].first &&
            point <= char32_chinese_japanese_korean[index].second)
            return true;

    return false;
}

std::string to_non_combining_form(const std::string& value)
{
    if (value.empty())
        return value;

    // utf32 ensures each word is a single unicode character.
    auto points = to_utf32(value);

    points.erase(std::remove_if(points.begin(), points.end(), is_combining),
        points.end());

    return to_utf8(points);
}

std::string to_non_diacritic_form(const std::string& value)
{
    if (value.empty())
        return value;

    // utf32 ensures each word is a single unicode character.
    auto points = to_utf32(value);

    points.erase(std::remove_if(points.begin(), points.end(), is_diacritic),
        points.end());

    return to_utf8(points);
}

/// Compress ascii whitespace and remove ascii spaces between cjk characters.
std::string to_compressed_form(const std::string& value)
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
    for (size_t point = 1; point < points.size() - 1u; point++)
    {
        if (!(is_ascii_whitespace(points[point]) &&
            is_chinese_japanese_or_korean(points[point - 1u]) &&
            is_chinese_japanese_or_korean(points[point + 1u])))
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
