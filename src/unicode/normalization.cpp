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

// Local helpers.
// ----------------------------------------------------------------------------

constexpr bool is_contained(char32_t value,
    const char32_interval& interval) NOEXCEPT
{
    return interval.first <= value && value <= interval.second;
}

constexpr char32_t unicode_max_code_point = 0x10ffffu;

// Hangul syllable constants (algorithmic decomposition/composition).
// unicode.org/reports/tr15/#Hangul
constexpr char32_t hangul_syllable_base  = 0xac00u;
constexpr char32_t hangul_leading_base   = 0x1100u;
constexpr char32_t hangul_vowel_base     = 0x1161u;
constexpr char32_t hangul_trailing_base  = 0x11a7u;
constexpr uint32_t hangul_leading_count  = 19;
constexpr uint32_t hangul_vowel_count    = 21;
constexpr uint32_t hangul_trailing_count = 28;
constexpr uint32_t hangul_nucleus_count  = hangul_vowel_count * hangul_trailing_count; // 588
constexpr uint32_t hangul_syllable_count = hangul_leading_count * hangul_nucleus_count; // 11172

// Unicode normalization helpers.
// ----------------------------------------------------------------------------

// Get canonical combining class (0 = starter).
static uint8_t get_ccc(char32_t point) NOEXCEPT
{
    if (point > unicode_max_code_point)
        return 0;
    // Two-level trie: level-1 selects the 128-entry block, level-2 resolves the entry.
    const auto data1 = unicode_data1[shift_right(point, 7)];
    const auto data2 = unicode_data2[shift_left(possible_wide_cast<size_t>(data1), 7)
        + bit_and(point, unmask_right<char32_t>(7))];
    return combining_index[data2];
}

// Decompose one code point into out (recursive, single-step per table entry).
static void decompose_one(std::u32string& out, char32_t point,
    bool compatible) NOEXCEPT
{
    // Hangul: algorithmic decomposition (same for NFD and NFKD).
    if (point >= hangul_syllable_base && point < hangul_syllable_base + hangul_syllable_count)
    {
        const auto syllable_index = possible_narrow_cast<uint32_t>(point - hangul_syllable_base);
        out.push_back(hangul_leading_base + syllable_index / hangul_nucleus_count);
        out.push_back(hangul_vowel_base + (syllable_index % hangul_nucleus_count) / hangul_trailing_count);
        const auto trailing_index = syllable_index % hangul_trailing_count;
        if (is_nonzero(trailing_index))
            out.push_back(hangul_trailing_base + trailing_index);
        return;
    }

    // Two-level trie lookup.
    if (point > unicode_max_code_point)
    {
        out.push_back(point);
        return;
    }
    // Two-level trie: level-1 selects the block, level-2 resolves the entry.
    const auto block = decomp_index1[shift_right(point, decomp_shift)];
    const auto index = decomp_index2[
        shift_left(possible_wide_cast<size_t>(block), decomp_shift)
        + bit_and(point, unmask_right<char32_t>(decomp_shift))];

    if (is_zero(index))
    {
        out.push_back(point);
        return;
    }

    const auto header        = decomp_pool[index];
    // Most significant bit flags a compatibility-only (NFKD) mapping.
    const auto is_compatible = get_left(header);
    // Bits 24-30: number of decomposition components (max 127).
    const auto count = bit_and(shift_right(header, 24), unmask_right<uint32_t>(7));

    // In NFD mode skip compatibility-only mappings.
    if (is_compatible && !compatible)
    {
        out.push_back(point);
        return;
    }

    // Recurse into each component.
    for (size_t component{}; component < count; ++component)
        decompose_one(out, possible_narrow_cast<char32_t>(decomp_pool[add1(index) + component]),
            compatible);
}

// Apply canonical ordering (UAX #15 section 3.11):
// Within each "combining sequence" (runs of CCC > 0), sort by CCC, stable.
static void canonical_order(std::u32string& sequence) NOEXCEPT
{
    const auto size = sequence.size();
    if (size < 2u)
        return;

    // Identify runs of combining characters and stable-sort each.
    size_t run_start{};
    while (run_start < size)
    {
        // Skip starters.
        while (run_start < size && is_zero(get_ccc(sequence[run_start])))
            ++run_start;

        // Find end of this combining run.
        size_t run_end = run_start;
        while (run_end < size && is_nonzero(get_ccc(sequence[run_end])))
            ++run_end;

        if (run_end > run_start + 1u)
        {
            std::stable_sort(sequence.begin() + run_start, sequence.begin() + run_end,
                [](char32_t a, char32_t b) NOEXCEPT
                {
                    return get_ccc(a) < get_ccc(b);
                });
        }
        run_start = run_end;
    }
}

// Canonical composition lookup (returns 0 if no composition found).
static char32_t comp_lookup(char32_t a, char32_t b) NOEXCEPT
{
    // Hangul L + V → LV
    if (a >= hangul_leading_base && a < hangul_leading_base + hangul_leading_count &&
        b >= hangul_vowel_base && b < hangul_vowel_base + hangul_vowel_count)
    {
        return hangul_syllable_base
            + (a - hangul_leading_base) * hangul_nucleus_count
            + (b - hangul_vowel_base) * hangul_trailing_count;
    }

    // Hangul LV + T → LVT
    if (a >= hangul_syllable_base && a < hangul_syllable_base + hangul_syllable_count &&
        is_zero((a - hangul_syllable_base) % hangul_trailing_count) &&
        b >  hangul_trailing_base  && b < hangul_trailing_base + hangul_trailing_count)
    {
        return a + (b - hangul_trailing_base);
    }

    // Binary search in comp_pairs (sorted by [a, b]).
    size_t lo{}, hi = comp_pairs_count;
    while (lo < hi)
    {
        const size_t mid  = (lo + hi) / 2u;
        const size_t base = mid * 3u;
        const auto   ca   = comp_pairs[base];
        const auto   cb   = comp_pairs[base + 1u];
        if (ca < a || (ca == a && cb < b))
            lo = mid + 1u;
        else if (ca > a || (ca == a && cb > b))
            hi = mid;
        else
            return possible_narrow_cast<char32_t>(comp_pairs[base + 2u]);
    }
    return 0u;
}

// Apply canonical composition pass (UAX #15 canonical composition algorithm).
static void compose(std::u32string& sequence) NOEXCEPT
{
    const size_t size = sequence.size();
    if (size < 2u)
        return;

    // We scan for each starter and try to compose with following combiners.
    size_t index{};
    while (index < sequence.size())
    {
        if (is_nonzero(get_ccc(sequence[index])))
        {
            ++index;
            continue;
        }

        // sequence[index] is a starter; scan forward for composable characters.
        uint8_t last_ccc{};
        size_t inner = add1(index);
        while (inner < sequence.size())
        {
            const uint8_t ccc = get_ccc(sequence[inner]);

            // A combining character is "blocked" if a character with equal or
            // higher CCC appeared between the starter and this character.
            const bool blocked = (last_ccc > 0u && last_ccc >= ccc);

            if (is_zero(ccc))
            {
                // sequence[inner] is also a starter. Try composition (e.g. Hangul L+V,
                // LV+T); if they compose, continue scanning; otherwise stop.
                if (!blocked)
                {
                    const char32_t c = comp_lookup(sequence[index], sequence[inner]);
                    if (is_nonzero(c))
                    {
                        sequence[index] = c;
                        sequence.erase(sequence.begin() + sign_cast<std::ptrdiff_t>(inner));
                        last_ccc = 0;
                        continue;
                    }
                }
                break; // Next starter that won't compose — stop.
            }

            if (!blocked)
            {
                const char32_t c = comp_lookup(sequence[index], sequence[inner]);
                if (is_nonzero(c))
                {
                    sequence[index] = c;
                    sequence.erase(sequence.begin() + sign_cast<std::ptrdiff_t>(inner));
                    last_ccc = 0; // Restart scan from just after new starter.
                    continue;
                }
            }

            last_ccc = ccc;
            ++inner;
        }
        ++index;
    }
}

// Full normalization (decompose → order → optionally compose).
static bool normal_form(std::string& value, bool compatible,
    bool recompose) NOEXCEPT
{
    const auto u32 = to_utf32(value);
    std::u32string out;
    out.reserve(u32.size() * 2u); // Most decompositions are small.

    for (const char32_t point : u32)
        decompose_one(out, point, compatible);

    canonical_order(out);

    if (recompose)
        compose(out);

    value = to_utf8(out);
    return true;
}

// Case folding helpers.
// ----------------------------------------------------------------------------

// Binary search in case_fold_pairs (sorted by from_cp).
// Returns folded code points written into out[], returns count (1-3).
static uint32_t fold_lower_one(char32_t point, char32_t out[3]) NOEXCEPT
{
    size_t lo{}, hi = case_fold_pairs_count;
    while (lo < hi)
    {
        const size_t mid  = (lo + hi) / 2u;
        const size_t base = mid * 5u;
        const auto   from = case_fold_pairs[base];
        if (from < possible_narrow_cast<uint32_t>(point))
            lo = mid + 1u;
        else if (from > possible_narrow_cast<uint32_t>(point))
            hi = mid;
        else
        {
            const uint32_t count = case_fold_pairs[base + 1u];
            out[0] = possible_narrow_cast<char32_t>(case_fold_pairs[base + 2u]);
            out[1] = possible_narrow_cast<char32_t>(case_fold_pairs[base + 3u]);
            out[2] = possible_narrow_cast<char32_t>(case_fold_pairs[base + 4u]);
            return count;
        }
    }
    out[0] = point;
    return 1u; // Identity (already lowercase or caseless).
}

static uint32_t fold_upper_one(char32_t point, char32_t out[3]) NOEXCEPT
{
    size_t lo{}, hi = upper_pairs_count;
    while (lo < hi)
    {
        const size_t mid  = (lo + hi) / 2u;
        const size_t base = mid * 5u;
        const auto   from = upper_pairs[base];
        if (from < possible_narrow_cast<uint32_t>(point))
            lo = mid + 1u;
        else if (from > possible_narrow_cast<uint32_t>(point))
            hi = mid;
        else
        {
            const uint32_t count = upper_pairs[base + 1u];
            out[0] = possible_narrow_cast<char32_t>(upper_pairs[base + 2u]);
            out[1] = possible_narrow_cast<char32_t>(upper_pairs[base + 3u]);
            out[2] = possible_narrow_cast<char32_t>(upper_pairs[base + 4u]);
            return count;
        }
    }
    out[0] = point;
    return 1u;
}

static bool apply_case(std::string& out, const std::string& in,
    bool lower) NOEXCEPT
{
    const auto u32 = to_utf32(in);
    std::u32string result;
    result.reserve(u32.size());

    char32_t buf[3]{};
    for (const char32_t point : u32)
    {
        const uint32_t count = lower ? fold_lower_one(point, buf)
                                     : fold_upper_one(point, buf);
        for (size_t index{}; index < count; ++index)
            result.push_back(buf[index]);
    }

    out = to_utf8(result);
    return true;
}

// Unicode case folding (embedded tables).
// ----------------------------------------------------------------------------

bool to_lower(std::string& value) NOEXCEPT
{
    if (is_ascii(value))
    {
        value = ascii_to_lower(value);
        return true;
    }

    return apply_case(value, value, true);
}

bool to_upper(std::string& value) NOEXCEPT
{
    if (is_ascii(value))
    {
        value = ascii_to_upper(value);
        return true;
    }

    return apply_case(value, value, false);
}

bool to_canonical_composition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, false, true);
}

bool to_canonical_decomposition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, false, false);
}

bool to_compatibility_composition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, true, true);
}

bool to_compatibility_decomposition(std::string& value) NOEXCEPT
{
    return is_ascii(value) || normal_form(value, true, false);
}

// No ICU dependency.
// ----------------------------------------------------------------------------

bool is_unicode(char32_t point) NOEXCEPT
{
    return point <= unicode_max_code_point;
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
    if (!is_unicode(point))
        return false;

    // github.com/python/cpython/blob/main/Modules/unicodedata.c
    // Two-level trie: level-1 selects the 128-entry block, level-2 resolves the entry.
    const auto data1 = unicode_data1[shift_right(point, 7)];
    const auto data2 = unicode_data2[shift_left(possible_wide_cast<size_t>(data1), 7)
        + bit_and(point, unmask_right<char32_t>(7))];
    return !is_zero(combining_index[data2]);
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
