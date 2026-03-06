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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * normalization_demo.cpp
 *
 * Demonstrates Unicode normalization and case folding as provided by
 * libbitcoin-system without an ICU dependency. All normalization is
 * performed via embedded tables generated from Python's unicodedata
 * module (Unicode 15.0.0).
 *
 * The functions demonstrated here are used internally by:
 *   - BIP-38 encrypted keys  (NFC passphrase normalization)
 *   - BIP-39 mnemonics       (NFKD passphrase normalization)
 *   - Electrum mnemonics     (NFKD + case fold + compress)
 *
 * Build within the project (after installing dependencies):
 *   cmake --build obj/<preset> --target libbitcoin-system-examples
 *
 * Note: BC_USE_LIBBITCOIN_MAIN and bc::system::main() are the project's
 * cross-platform entry-point wrappers that handle UTF-8 stdio on Windows.
 * For a standalone program outside the project replace them with a plain
 * int main() and remove the set_utf8_stdio() call.
 */

#include <cstdlib>
#include <ostream>
#include <string>
#include <bitcoin/system.hpp>

BC_USE_LIBBITCOIN_MAIN

// ---------------------------------------------------------------------------
// Helper: print a labeled before/after normalization result.
//
// Displays the string as both raw UTF-8 text and lowercase hex bytes so
// the result is unambiguous even for non-printable or look-alike code points.
// ---------------------------------------------------------------------------
static void show(std::ostream& out,
    const std::string& label,
    const std::string& before,
    const std::string& after,
    bool ok)
{
    using namespace bc::system;
    out << "  " << label << "\n";
    out << "    before : " << before
        << "  [" << encode_base16(before) << "]\n";
    out << "    after  : " << after
        << "  [" << encode_base16(after) << "]"
        << (ok ? "" : "  FAILED") << "\n\n";
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int bc::system::main(int /*argc*/, char* /*argv*/[])
{
    using namespace bc;
    using namespace bc::system;

    // On Windows, redirect stdin/stdout/stderr to UTF-8 so non-ASCII
    // characters print correctly. No-op on Linux/macOS.
    set_utf8_stdio();

    system::cout << "libbitcoin-system — Unicode normalization demo\n";
    system::cout << "Unicode 15.0.0 via embedded tables (no ICU required)\n";
    system::cout << std::string(60, '-') << "\n\n";

    bool all_ok = true;

    // -----------------------------------------------------------------------
    // 1. Case folding — to_lower / to_upper
    //
    // to_lower() applies Unicode full case folding (not just ASCII).
    // This means a single input code point can produce multiple output code
    // points (e.g. ß → ss). The function returns false only if the input is
    // not valid UTF-8.
    // -----------------------------------------------------------------------
    system::cout << "1. Case folding\n\n";

    {
        // ASCII — straightforward
        auto value = std::string{ "Hello, World!" };
        auto before = value;
        const bool ok = to_lower(value);
        all_ok &= ok && value == "hello, world!";
        show(system::cout, "to_lower (ASCII)", before, value, ok);
    }
    {
        // ß (U+00DF) is a single code point that folds to two: ss
        auto value = to_string(base16_chunk("c39f")); // ß in UTF-8
        auto before = value;
        const bool ok = to_lower(value);
        all_ok &= ok && value == "ss";
        show(system::cout, "to_lower  ß → ss  (multi-char fold)", before, value, ok);
    }
    {
        // Ohm sign (U+2126) and Greek Omega (U+03A9) are case-equivalent
        auto value = to_string(base16_chunk("e284a6")); // Ω Ohm sign
        auto before = value;
        const bool ok = to_lower(value);
        all_ok &= ok;
        show(system::cout, "to_lower  Ω (Ohm U+2126) → ω (U+03C9)", before, value, ok);
    }
    {
        // to_upper on a mixed Latin string
        auto value = std::string{ "café" };
        auto before = value;
        const bool ok = to_upper(value);
        all_ok &= ok;
        show(system::cout, "to_upper  café → CAFÉ", before, value, ok);
    }

    // -----------------------------------------------------------------------
    // 2. Canonical decomposition and composition (NFD / NFC)
    //
    // NFD breaks precomposed characters into base + combining sequences.
    // NFC reassembles them. The two operations are inverses of each other
    // for well-formed Unicode text.
    //
    // Used by BIP-38 for passphrase normalization (NFC).
    // -----------------------------------------------------------------------
    system::cout << "2. Canonical decomposition / composition (NFD / NFC)\n\n";

    {
        // é (U+00E9) is a single precomposed code point.
        // NFD splits it into: e (U+0065) + combining acute accent (U+0301).
        auto value = to_string(base16_chunk("c3a9")); // é
        auto before = value;
        const bool ok = to_canonical_decomposition(value);
        all_ok &= ok;
        show(system::cout, "NFD  é (U+00E9) → e + combining-acute", before, value, ok);
    }
    {
        // Reverse: base + combining acute → precomposed é
        auto value = to_string(base16_chunk("65cc81")); // e + combining acute
        auto before = value;
        const bool ok = to_canonical_composition(value);
        all_ok &= ok;
        show(system::cout, "NFC  e + combining-acute → é (U+00E9)", before, value, ok);
    }
    {
        // Round-trip: NFD then NFC must restore the original string exactly.
        const auto original = std::string{ "Ångström" };
        auto value = original;
        bool ok = to_canonical_decomposition(value);
        ok &= to_canonical_composition(value);
        ok &= (value == original);
        all_ok &= ok;
        system::cout << "  NFD → NFC round-trip  \"Ångström\"\n";
        system::cout << "    result : " << (ok ? "PASS" : "FAIL") << "\n\n";
    }
    {
        // Hangul: algorithmic decomposition (no table entry needed).
        // 가 (U+AC00) = L-jamo (U+1100) + V-jamo (U+1161)
        auto value = to_string(base16_chunk("eab080")); // 가
        auto before = value;
        const bool ok = to_canonical_decomposition(value);
        all_ok &= ok;
        show(system::cout, "NFD  가 (U+AC00) → ᄀ + ᅡ  (Hangul L+V)", before, value, ok);
    }
    {
        // Hangul composition: L + V → LV syllable
        auto value = to_string(base16_chunk("e18480e185a1")); // ᄀ + ᅡ
        auto before = value;
        const bool ok = to_canonical_composition(value);
        all_ok &= ok;
        show(system::cout, "NFC  ᄀ + ᅡ → 가 (U+AC00)  (Hangul L+V)", before, value, ok);
    }

    // -----------------------------------------------------------------------
    // 3. Compatibility decomposition and composition (NFKD / NFKC)
    //
    // NFKD additionally decomposes characters that are semantically
    // equivalent but visually distinct (ligatures, superscripts, etc.).
    //
    // Used by BIP-39 and Electrum for passphrase normalization (NFKD).
    // The ideographic space (U+3000) is particularly important for Japanese
    // BIP-39 word lists, where it serves as the word separator.
    // -----------------------------------------------------------------------
    system::cout << "3. Compatibility decomposition / composition (NFKD / NFKC)\n\n";

    {
        // fi ligature (U+FB01) has no canonical decomposition but does have
        // a compatibility decomposition: f (U+0066) + i (U+0069).
        auto value = to_string(base16_chunk("efac81")); // ﬁ
        auto before = value;
        const bool ok = to_compatibility_decomposition(value);
        all_ok &= ok && value == "fi";
        show(system::cout, "NFKD  ﬁ (fi ligature U+FB01) → fi", before, value, ok);
    }
    {
        // Ideographic space (U+3000) → ASCII space (U+0020) under NFKD.
        // This is how BIP-39 Japanese passphrases are normalized.
        auto value = to_string(base16_chunk("e38080")); // U+3000
        auto before = value;
        const bool ok = to_compatibility_decomposition(value);
        all_ok &= ok && value == " ";
        show(system::cout, "NFKD  ideographic space (U+3000) → ASCII space", before, value, ok);
    }
    {
        // BIP-39 Japanese: word list uses ideographic space as separator.
        // After NFKD the sentence uses ordinary ASCII spaces.
        const auto ideo  = "あいこくしん\xe3\x80\x80あいさつ"; // U+3000 separator
        const auto ascii = "あいこくしん あいさつ";            // U+0020 separator
        auto value = std::string{ ideo };
        const bool ok = to_compatibility_decomposition(value) && value == ascii;
        all_ok &= ok;
        system::cout << "  NFKD  BIP-39 Japanese sentence (ideographic → ASCII space)\n";
        system::cout << "    before : " << ideo  << "\n";
        system::cout << "    after  : " << value << "\n";
        system::cout << "    result : " << (ok ? "PASS" : "FAIL") << "\n\n";
    }

    // -----------------------------------------------------------------------
    // 4. BIP-38 reference vector
    //
    // BIP-38 test vector passphrase normalization uses NFC.
    // Input bytes (as code points): U+03D2 U+0301 U+0000 U+10000 U+1F4A9
    //
    // Source: github.com/bitcoin/bips/blob/master/bip-0038.mediawiki
    // -----------------------------------------------------------------------
    system::cout << "4. BIP-38 reference vector\n\n";

    {
        auto decomposed = to_string(base16_chunk("cf92cc8100f0909080f09f92a9"));
        auto before = decomposed;
        const bool ok = to_canonical_composition(decomposed);
        // U+03D2 + U+0301 compose to U+03D3 (ϓ, Greek upsilon with acute hook)
        // U+03D3 = CF 93 in UTF-8; remaining code points are unchanged.
        const auto expected = std::string{ "cf9300f0909080f09f92a9" };
        all_ok &= ok;
        show(system::cout, "NFC  BIP-38 decomposed passphrase bytes → composed", before, decomposed, ok);
        system::cout << "  expected hex : " << expected << "\n";
        system::cout << "  result       : " << (encode_base16(decomposed) == expected ? "PASS" : "FAIL") << "\n\n";
    }

    // -----------------------------------------------------------------------
    // Summary
    // -----------------------------------------------------------------------
    system::cout << std::string(60, '-') << "\n";
    system::cout << "Overall result: " << (all_ok ? "ALL PASS" : "FAILURES DETECTED") << "\n";

    return all_ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
