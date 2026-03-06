/**
 * Copyright (c) 2011-2026 libbitcoin-system developers (see AUTHORS)
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

// Hangul syllable constants (algorithmic decomposition/composition).
// unicode.org/reports/tr15/#Hangul
constexpr char32_t hangul_sbase  = 0xAC00u;
constexpr char32_t hangul_lbase  = 0x1100u;
constexpr char32_t hangul_vbase  = 0x1161u;
constexpr char32_t hangul_tbase  = 0x11A7u;
constexpr uint32_t hangul_lcount = 19u;
constexpr uint32_t hangul_vcount = 21u;
constexpr uint32_t hangul_tcount = 28u;
constexpr uint32_t hangul_ncount = hangul_vcount * hangul_tcount; // 588
constexpr uint32_t hangul_scount = hangul_lcount * hangul_ncount; // 11172

// Unicode normalization helpers.
// ----------------------------------------------------------------------------

// Get canonical combining class (0 = starter).
static uint8_t get_ccc(char32_t point) NOEXCEPT
{
    if (point > 0x10FFFFu)
        return 0;
    const auto data1 = unicode_data1[(point >> 7)];
    const auto data2 = unicode_data2[(data1 << 7) + (point & 0x7Fu)];
    return combining_index[data2];
}

// Decompose one code point into out (recursive, single-step per table entry).
static void decompose_one(std::u32string& out, char32_t cp,
    bool compat) NOEXCEPT
{
    // Hangul: algorithmic decomposition (same for NFD and NFKD).
    if (cp >= hangul_sbase && cp < hangul_sbase + hangul_scount)
    {
        const auto si = static_cast<uint32_t>(cp - hangul_sbase);
        out.push_back(hangul_lbase + si / hangul_ncount);
        out.push_back(hangul_vbase + (si % hangul_ncount) / hangul_tcount);
        const auto ti = si % hangul_tcount;
        if (ti != 0u)
            out.push_back(hangul_tbase + ti);
        return;
    }

    // Two-level trie lookup.
    if (cp > 0x10FFFFu)
    {
        out.push_back(cp);
        return;
    }
    const auto block = decomp_index1[cp >> decomp_shift];
    const auto idx   = decomp_index2[
        (static_cast<size_t>(block) << decomp_shift) + (cp & 0x7Fu)];

    if (idx == 0u)
    {
        out.push_back(cp);
        return;
    }

    const auto header    = decomp_pool[idx];
    const auto is_compat = (header & 0x80000000u) != 0u;
    const auto count     = (header >> 24) & 0x7Fu;

    // In NFD mode skip compatibility-only mappings.
    if (is_compat && !compat)
    {
        out.push_back(cp);
        return;
    }

    // Recurse into each component.
    for (uint32_t i = 0u; i < count; ++i)
        decompose_one(out, static_cast<char32_t>(decomp_pool[idx + 1u + i]),
            compat);
}

// Apply canonical ordering (UAX #15 section 3.11):
// Within each "combining sequence" (runs of CCC > 0), sort by CCC, stable.
static void canonical_order(std::u32string& seq) NOEXCEPT
{
    const auto n = seq.size();
    if (n < 2u)
        return;

    // Identify runs of combining characters and stable-sort each.
    size_t run_start = 0u;
    while (run_start < n)
    {
        // Skip starters.
        while (run_start < n && get_ccc(seq[run_start]) == 0u)
            ++run_start;

        // Find end of this combining run.
        size_t run_end = run_start;
        while (run_end < n && get_ccc(seq[run_end]) != 0u)
            ++run_end;

        if (run_end > run_start + 1u)
        {
            std::stable_sort(seq.begin() + run_start, seq.begin() + run_end,
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
    if (a >= hangul_lbase && a < hangul_lbase + hangul_lcount &&
        b >= hangul_vbase && b < hangul_vbase + hangul_vcount)
    {
        return hangul_sbase
            + (a - hangul_lbase) * hangul_ncount
            + (b - hangul_vbase) * hangul_tcount;
    }

    // Hangul LV + T → LVT
    if (a >= hangul_sbase && a < hangul_sbase + hangul_scount &&
        (a - hangul_sbase) % hangul_tcount == 0u &&
        b >  hangul_tbase  && b < hangul_tbase + hangul_tcount)
    {
        return a + (b - hangul_tbase);
    }

    // Binary search in comp_pairs (sorted by [a, b]).
    size_t lo = 0u, hi = comp_pairs_count;
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
            return static_cast<char32_t>(comp_pairs[base + 2u]);
    }
    return 0u;
}

// Apply canonical composition pass (UAX #15 canonical composition algorithm).
static void compose(std::u32string& seq) NOEXCEPT
{
    const size_t n = seq.size();
    if (n < 2u)
        return;

    // We scan for each starter and try to compose with following combiners.
    size_t i = 0u;
    while (i < seq.size())
    {
        if (get_ccc(seq[i]) != 0u)
        {
            ++i;
            continue;
        }

        // seq[i] is a starter; scan forward for composable characters.
        uint8_t last_ccc = 0u;
        size_t j = i + 1u;
        while (j < seq.size())
        {
            const uint8_t ccc = get_ccc(seq[j]);

            // A combining character is "blocked" if a character with equal or
            // higher CCC appeared between the starter and this character.
            const bool blocked = (last_ccc > 0u && last_ccc >= ccc);

            if (ccc == 0u)
            {
                // seq[j] is also a starter. Try composition (e.g. Hangul L+V,
                // LV+T); if they compose, continue scanning; otherwise stop.
                if (!blocked)
                {
                    const char32_t c = comp_lookup(seq[i], seq[j]);
                    if (c != 0u)
                    {
                        seq[i] = c;
                        seq.erase(seq.begin() + static_cast<std::ptrdiff_t>(j));
                        last_ccc = 0u;
                        continue;
                    }
                }
                break; // Next starter that won't compose — stop.
            }

            if (!blocked)
            {
                const char32_t c = comp_lookup(seq[i], seq[j]);
                if (c != 0u)
                {
                    seq[i] = c;
                    seq.erase(seq.begin() + static_cast<std::ptrdiff_t>(j));
                    last_ccc = 0u; // Restart scan from just after new starter.
                    continue;
                }
            }

            last_ccc = ccc;
            ++j;
        }
        ++i;
    }
}

// Full normalization (decompose → order → optionally compose).
static bool normal_form(std::string& value, bool compat,
    bool recompose) NOEXCEPT
{
    const auto u32 = to_utf32(value);
    std::u32string out;
    out.reserve(u32.size() * 2u); // Most decompositions are small.

    for (const char32_t cp : u32)
        decompose_one(out, cp, compat);

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
static uint32_t fold_lower_one(char32_t cp, char32_t out[3]) NOEXCEPT
{
    size_t lo = 0u, hi = case_fold_pairs_count;
    while (lo < hi)
    {
        const size_t mid  = (lo + hi) / 2u;
        const size_t base = mid * 5u;
        const auto   from = case_fold_pairs[base];
        if (from < static_cast<uint32_t>(cp))
            lo = mid + 1u;
        else if (from > static_cast<uint32_t>(cp))
            hi = mid;
        else
        {
            const uint32_t n = case_fold_pairs[base + 1u];
            out[0] = static_cast<char32_t>(case_fold_pairs[base + 2u]);
            out[1] = static_cast<char32_t>(case_fold_pairs[base + 3u]);
            out[2] = static_cast<char32_t>(case_fold_pairs[base + 4u]);
            return n;
        }
    }
    out[0] = cp;
    return 1u; // Identity (already lowercase or caseless).
}

static uint32_t fold_upper_one(char32_t cp, char32_t out[3]) NOEXCEPT
{
    size_t lo = 0u, hi = upper_pairs_count;
    while (lo < hi)
    {
        const size_t mid  = (lo + hi) / 2u;
        const size_t base = mid * 5u;
        const auto   from = upper_pairs[base];
        if (from < static_cast<uint32_t>(cp))
            lo = mid + 1u;
        else if (from > static_cast<uint32_t>(cp))
            hi = mid;
        else
        {
            const uint32_t n = upper_pairs[base + 1u];
            out[0] = static_cast<char32_t>(upper_pairs[base + 2u]);
            out[1] = static_cast<char32_t>(upper_pairs[base + 3u]);
            out[2] = static_cast<char32_t>(upper_pairs[base + 4u]);
            return n;
        }
    }
    out[0] = cp;
    return 1u;
}

static bool apply_case(std::string& out, const std::string& in,
    bool lower) NOEXCEPT
{
    const auto u32 = to_utf32(in);
    std::u32string result;
    result.reserve(u32.size());

    char32_t buf[3]{};
    for (const char32_t cp : u32)
    {
        const uint32_t n = lower ? fold_lower_one(cp, buf)
                                 : fold_upper_one(cp, buf);
        for (uint32_t i = 0u; i < n; ++i)
            result.push_back(buf[i]);
    }

    out = to_utf8(result);
    return true;
}

// ICU dependency (ascii supported, otherwise false if HAVE_ICU not defined).
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
