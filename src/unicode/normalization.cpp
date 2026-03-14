/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/ascii.hpp>
#include <bitcoin/system/unicode/code_points.hpp>
#include <bitcoin/system/unicode/conversion.hpp>
#include <bitcoin/system/unicode/unicode_tables.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Constants.
// ----------------------------------------------------------------------------

constexpr char32_t max_unicode_point = 0x10ffff;

// Hangul syllable constants (unicode.org/reports/tr15/#Hangul).
namespace hangul
{
    constexpr char32_t syllable_base = 0xac00;
    constexpr char32_t leading_base = 0x1100;
    constexpr char32_t vowel_base = 0x1161;
    constexpr char32_t trailing_base = 0x11a7;
    constexpr uint32_t leading_count = 19;
    constexpr uint32_t vowel_count = 21;
    constexpr uint32_t trailing_count = 28;
    constexpr uint32_t nucleus_count = vowel_count * trailing_count;
    constexpr uint32_t syllable_count = leading_count * nucleus_count;
}

// Unicode normalization helpers.
// ----------------------------------------------------------------------------

constexpr size_t components(uint32_t header) NOEXCEPT
{
    // Bits 24-30 is the number of decomposition components.
    constexpr size_t discard = 24;
    constexpr size_t visible = add1(30 - discard);
    return bit_and(shift_right(header, discard),
        unmask_right<uint32_t>(visible));
}

constexpr bool is_compatible(uint32_t header) NOEXCEPT
{
    // Most significant bit flags a compatibility-only (NFKD) mapping.
    return get_left(header);
}

static uint32_t decompose(char32_t point) NOEXCEPT
{
    constexpr size_t shift = 7;

    // Two-level trie: level-1 selects 128-entry block, level-2 resolves entry.
    const auto block = decomposition_index1[shift_right(point, shift)];
    return decomposition_index2[shift_left<uint32_t>(block, shift) +
        bit_and(point, unmask_right<char32_t>(shift))];
}

static void decompose_one(std::u32string& out, char32_t point,
    bool compatible) NOEXCEPT
{
    using namespace hangul;

    // Hangul: algorithmic decomposition (same for NFD and NFKD).
    if (point >= syllable_base &&
        point < syllable_base + syllable_count)
    {
        const auto syllable = point - syllable_base;
        out.push_back(leading_base + syllable / nucleus_count);

        const auto modulo = (syllable % nucleus_count);
        out.push_back(vowel_base + modulo / trailing_count);

        const auto trailing = syllable % trailing_count;
        if (is_nonzero(trailing))
            out.push_back(trailing_base + trailing);

        return;
    }

    if (point > max_unicode_point)
    {
        out.push_back(point);
        return;
    }

    const auto index = decompose(point);
    if (is_zero(index))
    {
        out.push_back(point);
        return;
    }

    const auto header = decomposition_pool[index];

    // Skip compatibility-only mappings in NFD mode.
    if (is_compatible(header) && !compatible)
    {
        out.push_back(point);
        return;
    }

    // Recurse into each component.
    for (size_t component{}; component < components(header); ++component)
        decompose_one(out, decomposition_pool[add1(index) + component],
            compatible);
}

static uint8_t get_canonical(char32_t point) NOEXCEPT
{
    if (point > max_unicode_point)
        return {};

    // Two-level trie: level-1 selects 128-entry block, level-2 resolves entry.
    const auto data1 = unicode_data1[shift_right(point, 7)];
    const auto data2 = unicode_data2[shift_left<uint32_t>(data1, 7) +
        bit_and(point, unmask_right<char32_t>(7))];

    return combining_index[data2];
}

// Apply canonical ordering (UAX #15 section 3.11).
static void canonical_order(std::u32string& sequence) NOEXCEPT
{
    const auto size = sequence.size();
    if (size < two)
        return;

    size_t start{};
    while (start < size)
    {
        // Skip starters.
        while (start < size && is_zero(get_canonical(sequence[start])))
            ++start;

        // Find end of combining run.
        auto end{ start };
        while (end < size && !is_zero(get_canonical(sequence[end])))
            ++end;

        // Stable-sort combining characters.
        if (end > add1(start))
        {
            const auto at = std::next(sequence.begin(), start);
            const auto to = std::next(sequence.begin(), end);
            std::stable_sort(at, to, [](auto left, auto right) NOEXCEPT
            {
                return get_canonical(left) < get_canonical(right);
            });
        }

        start = end;
    }
}

// Canonical composition lookup (0 if no composition found).
static char32_t composition_lookup(char32_t a, char32_t b) NOEXCEPT
{
    using namespace hangul;

    // Hangul L + V -> LV
    if ((a >= leading_base) &&
        (a < leading_base + leading_count) &&
        (b >= vowel_base) &&
        (b < vowel_base + vowel_count))
    {
        return syllable_base
            + ((a - leading_base) * nucleus_count)
            + ((b - vowel_base) * trailing_count);
    }

    // Hangul LV + T -> LVT
    if ((a >= syllable_base) &&
        (a < syllable_base + syllable_count) &&
        (is_zero((a - syllable_base) % trailing_count)) &&
        (b > trailing_base) &&
        (b < trailing_base + trailing_count))
    {
        return a + (b - trailing_base);
    }

    // Binary search in composition_pairs (sorted by [a, b]).
    size_t lo{}, hi{ composition_pairs_count };
    while (lo < hi)
    {
        const auto middle = to_half(lo + hi);
        const auto base = middle * 3u;
        const auto alpha = composition_pairs[base];
        const auto bravo = composition_pairs[add1(base)];

        if (alpha < a || (alpha == a && bravo < b))
        {
            lo = add1(middle);
        }
        else if (alpha > a || (alpha == a && bravo > b))
        {
            hi = middle;
        }
        else
        {
            return composition_pairs[base + 2u];
        }
    }

    return {};
}

// Apply canonical composition pass (UAX #15 canonical composition algorithm).
static void compose(std::u32string& sequence) NOEXCEPT
{
    const size_t size = sequence.size();
    if (size < two)
        return;

    for (size_t index{}; index < sequence.size();)
    {
        // Skip non-starters.
        if (!is_zero(get_canonical(sequence[index])))
        {
            ++index;
            continue;
        }

        uint8_t last{};

        // Scan for composable characters.
        for (auto inner{ add1(index) }; inner < sequence.size();)
        {
            const auto current = get_canonical(sequence[inner]);

            // Combining char "blocked" if char >= ccc between it and starter.
            const auto blocked = is_nonzero(last) && last >= current;

            if (is_zero(current))
            {
                if (!blocked)
                {
                    // sequence[inner] is also a starter, try composition.
                    const auto point = composition_lookup(sequence[index],
                        sequence[inner]);

                    if (is_nonzero(point))
                    {
                        sequence[index] = point;
                        sequence.erase(std::next(sequence.begin(), inner));
                        last = {};
                        continue;
                    }
                }

                // Stop on next starter that won't compose.
                break;
            }

            if (!blocked)
            {
                const auto point = composition_lookup(sequence[index],
                    sequence[inner]);

                if (is_nonzero(point))
                {
                    sequence[index] = point;
                    sequence.erase(std::next(sequence.begin(), inner));
                    last = {};
                    continue;
                }
            }

            last = current;
            ++inner;
        }

        ++index;
    }
}

static void normal_form(std::string& value, bool compatible,
    bool recompose) NOEXCEPT
{
    const auto points = to_utf32(value);
    std::u32string out{};

    // Most decompositions are small so reserve * two.
    out.reserve(points.size() * two);
    for (const auto point: points)
        decompose_one(out, point, compatible);

    canonical_order(out);
    if (recompose)
        compose(out);

    value = to_utf8(out);
}

// Case folding helpers.
// ----------------------------------------------------------------------------

// msvc is flagging array below indexation as pointer math.
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

// Binary search in case_fold_pairs (sorted by from_cp).
// Returns folded code points written into out[], returns count (1-3).
static uint32_t fold_lower_one(char32_t point, char32_t out[3]) NOEXCEPT
{
    size_t lo{}, hi{ case_fold_pairs_count };
    while (lo < hi)
    {
        const auto middle = to_half(lo + hi);
        const auto base = middle * 5u;
        const auto from = case_fold_pairs[base];

        if (from < point)
        {
            lo = add1(middle);
        }
        else if (from > point)
        {
            hi = middle;
        }
        else
        {
            const auto count = case_fold_pairs[add1(base)];
            out[0] = case_fold_pairs[base + 2u];
            out[1] = case_fold_pairs[base + 3u];
            out[2] = case_fold_pairs[base + 4u];
            return count;
        }
    }

    out[0] = point;
    return 1u;
}

static uint32_t fold_upper_one(char32_t point, char32_t out[3]) NOEXCEPT
{
    size_t lo{}, hi{ upper_pairs_count };
    while (lo < hi)
    {
        const auto middle  = to_half(lo + hi);
        const auto base = middle * 5_size;
        const auto from = upper_pairs[base];

        if (from < point)
        {
            lo = add1(middle);
        }
        else if (from > point)
        {
            hi = middle;
        }
        else
        {
            const auto count = upper_pairs[add1(base)];
            out[0] = upper_pairs[base + 2u];
            out[1] = upper_pairs[base + 3u];
            out[2] = upper_pairs[base + 4u];
            return count;
        }
    }

    out[0] = point;
    return 1u;
}

BC_POP_WARNING()

static std::string apply_case(const std::string& in, bool lower) NOEXCEPT
{
    const auto points = to_utf32(in);
    std::u32string out{};
    out.reserve(points.size());

    char32_t buffer[3]{};
    for (const auto point: points)
    {
        const auto count = lower ?
            fold_lower_one(point, &buffer[0]) :
            fold_upper_one(point, &buffer[0]);

        for (size_t index{}; index < count; ++index)
            out.push_back(buffer[index]);
    }

    return to_utf8(out);
}

// Unicode case folding.
// ----------------------------------------------------------------------------

void to_lower(std::string& value) NOEXCEPT
{
    value = is_ascii(value) ? ascii_to_lower(value) : apply_case(value, true);
}

void to_upper(std::string& value) NOEXCEPT
{
    value = is_ascii(value) ? ascii_to_upper(value) : apply_case(value, false);
}

void to_canonical_composition(std::string& value) NOEXCEPT
{
    if (!is_ascii(value)) normal_form(value, false, true);
}

void to_canonical_decomposition(std::string& value) NOEXCEPT
{
    if (!is_ascii(value)) normal_form(value, false, false);
}

void to_compatibility_composition(std::string& value) NOEXCEPT
{
    if (!is_ascii(value)) normal_form(value, true, true);
}

void to_compatibility_decomposition(std::string& value) NOEXCEPT
{
    if (!is_ascii(value)) normal_form(value, true, false);
}

// Character tests (no conversions).
// ----------------------------------------------------------------------------

bool is_unicode(char32_t point) NOEXCEPT
{
    return point <= max_unicode_point;
}

bool is_separator(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index{}; index < char32_separators_count; ++index)
        if (point == char32_separators[index])
            return true;

    return false;
}

bool is_whitespace(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index{}; index < char32_whitespace_count; ++index)
        if (point == char32_whitespace[index])
            return true;

    return false;
}

bool is_combining(char32_t point) NOEXCEPT
{
    // github.com/python/cpython/blob/main/Modules/unicodedata.c
    return is_unicode(point) && !is_zero(get_canonical(point));
}

// local
constexpr bool is_contained(char32_t value,
    const char32_interval& interval) NOEXCEPT
{
    return interval.first <= value && value <= interval.second;
}

bool is_diacritic(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index{}; index < char32_diacritics_count; ++index)
        if (is_contained(point, char32_diacritics[index]))
            return true;

    return false;
}

bool is_chinese_japanese_or_korean(char32_t point) NOEXCEPT
{
    if (!is_unicode(point))
        return false;

    for (size_t index{}; index < char32_chinese_japanese_korean_count; ++index)
        if (is_contained(point, char32_chinese_japanese_korean[index]))
            return true;

    return false;
}

bool has_whitespace(const std::string& value) NOEXCEPT
{
    if (value.empty())
        return false;

    const auto points = to_utf32(value);
    return std::any_of(points.begin(), points.end(),
        [](char32_t character) NOEXCEPT
        {
            return is_whitespace(character);
        });
}

// Conversions.
// ----------------------------------------------------------------------------

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
    for (auto point{ one }; point < sub1(points.size()); point++)
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

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
