#!/usr/bin/env python3
"""
Generate Unicode normalization tables for libbitcoin-system.

Source: Python's unicodedata module (tracks the Unicode standard version
        bundled with each Python release — python.org/downloads/release/).
        Unicode version used is reported at generation time via unicodedata.unidata_version.
Output: src/unicode/unicode_tables.cpp + include/bitcoin/system/unicode/unicode_tables.hpp

Tables generated:
  - Decomposition trie (decomp_index1, decomp_index2, decomp_pool) for NFD/NFKD
  - Composition pairs (comp_pairs) for NFC/NFKC
  - Case folding table (case_fold_pairs) for to_lower/to_upper

Run from the repository root:
  python3 tools/generate_unicode_tables.py
"""

import unicodedata
import sys
import os

SHIFT = 7  # Same as existing unicode_data1/data2 tables

LICENSE_HEADER = """\
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
"""

# ── helpers ──────────────────────────────────────────────────────────────────

def format_array(name, typ, data, cols=16):
    """Format a C++ array definition."""
    lines = [f"const {typ} {name}[]"]
    lines.append("{")
    for i in range(0, len(data), cols):
        chunk = data[i:i + cols]
        if typ == "uint32_t":
            row = ", ".join(f"0x{v:08x}u" for v in chunk)
        elif typ == "uint16_t":
            row = ", ".join(f"0x{v:04x}u" for v in chunk)
        else:
            row = ", ".join(f"0x{v:02x}u" for v in chunk)
        lines.append(f"    {row},")
    lines.append("};")
    return "\n".join(lines)

def splitbins(data, shift):
    """Compress data into two-level index using given shift."""
    block_size = 1 << shift
    # Build unique blocks
    blocks = {}
    index1 = []
    index2_flat = []

    for i in range(0, len(data), block_size):
        block = tuple(data[i:i + block_size])
        if block not in blocks:
            blocks[block] = len(index2_flat)
            index2_flat.extend(block)
        index1.append(blocks[block] >> shift)

    return index1, index2_flat

# ── decomposition ─────────────────────────────────────────────────────────────

def build_decomp_tables():
    """
    Build the decomposition trie.

    decomp_pool layout (uint32_t array):
        index 0: sentinel (no decomposition)
        each entry: header word followed by code points
            header = (is_compat << 31) | (count << 24)
            where count = number of code point words that follow
            is_compat: 1 = compatibility, 0 = canonical

    decomp_index2 values: index into decomp_pool, 0 = identity (no decomp)
    """
    # Collect single-step decompositions
    raw = {}  # cp -> (is_compat: bool, [target_cps])
    for cp in range(0x110000):
        try:
            c = chr(cp)
        except (ValueError, OverflowError):
            continue
        d = unicodedata.decomposition(c)
        if not d:
            continue
        if d.startswith('<'):
            parts = d.split()
            cps = [int(x, 16) for x in parts if not x.startswith('<')]
            raw[cp] = (True, cps)
        else:
            cps = [int(x, 16) for x in d.split()]
            raw[cp] = (False, cps)

    # Build pool (deduplicate identical sequences)
    pool = [0]  # index 0 = sentinel
    seq_cache = {}  # (is_compat, tuple(cps)) -> pool_index
    cp_to_pool = {}  # cp -> pool_index

    for cp, (is_compat, cps) in sorted(raw.items()):
        key = (is_compat, tuple(cps))
        if key not in seq_cache:
            idx = len(pool)
            header = (0x80000000 if is_compat else 0) | (len(cps) << 24)
            pool.append(header)
            pool.extend(cps)
            seq_cache[key] = idx
        cp_to_pool[cp] = seq_cache[key]

    # Build flat per-code-point index (0x110000 entries)
    flat = [0] * 0x110000
    for cp, idx in cp_to_pool.items():
        flat[cp] = idx

    # Compress into two-level trie
    index1, index2 = splitbins(flat, SHIFT)

    return pool, index1, index2

# ── composition ──────────────────────────────────────────────────────────────

def build_comp_pairs():
    """
    Build sorted composition pair table.
    Each entry: (starter_cp, combining_cp, composed_cp) as uint32_t[3].
    Used for NFC/NFKC composition step.
    Hangul composition is handled algorithmically.
    """
    pairs = []
    for cp in range(0x110000):
        try:
            c = chr(cp)
        except (ValueError, OverflowError):
            continue
        d = unicodedata.decomposition(c)
        if not d or d.startswith('<'):
            continue
        parts = [int(x, 16) for x in d.split()]
        if len(parts) != 2:
            continue
        a, b = parts
        # Verify it actually composes back (excludes composition exclusions)
        composed = unicodedata.normalize('NFC', chr(a) + chr(b))
        if len(composed) == 1 and ord(composed[0]) == cp:
            pairs.append((a, b, cp))

    pairs.sort()  # sort by (starter, combining) for binary search
    return pairs

# ── case folding ──────────────────────────────────────────────────────────────

def build_case_fold_table():
    """
    Build case fold table for to_lower.
    Uses Python str.casefold() which implements Unicode full case folding
    (C + F folds from CaseFolding.txt, no Turkish dotless-i / T folds).

    Each non-trivial entry stored as:
        from_cp  (uint32_t)
        n        (uint32_t: number of target code points, 1-3)
        to_cp[3] (uint32_t[3]: target code points, zero-padded)

    Sorted by from_cp for binary search.
    Also build to_upper table similarly using str.upper().
    """
    lower_entries = []
    upper_entries = []

    for cp in range(0x110000):
        try:
            c = chr(cp)
        except (ValueError, OverflowError):
            continue

        folded = c.casefold()
        if folded != c:
            cps = [ord(x) for x in folded]
            lower_entries.append((cp, cps))

        uppered = c.upper()
        if uppered != c and len(uppered) == 1:
            # Only store simple 1:1 uppercase mappings
            upper_entries.append((cp, [ord(uppered[0])]))

    return lower_entries, upper_entries

# ── generators ────────────────────────────────────────────────────────────────

def generate_cpp(pool, index1, index2, comp_pairs, lower_entries, upper_entries):
    """Generate the .cpp source file."""
    lines = [LICENSE_HEADER]
    lines.append("// Generated by tools/generate_unicode_tables.py")
    lines.append(f"// Unicode version: {unicodedata.unidata_version} (Python {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro})")
    lines.append("// To update: install a Python version with the desired Unicode data and re-run.")
    lines.append("")
    lines.append("#include <bitcoin/system/unicode/unicode_tables.hpp>")
    lines.append("")
    lines.append("namespace libbitcoin {")
    lines.append("namespace system {")
    lines.append("")

    # ── decomposition pool ───────────────────────────────────────────────────
    lines.append("// Decomposition pool: header word + code points per entry.")
    lines.append("// header = (is_compat << 31) | (count << 24)")
    lines.append("// is_compat: 1=compatibility decomp, 0=canonical decomp")
    lines.append("// index 0 = sentinel (no decomposition = identity)")
    lines.append(format_array("decomp_pool", "uint32_t", pool, 8))
    lines.append("")

    # ── decomposition trie level 1 ───────────────────────────────────────────
    lines.append("// Level-1 decomposition trie (indexed by code_point >> 7).")
    lines.append("// Value * 128 + (code_point & 0x7f) indexes decomp_index2.")
    lines.append(format_array("decomp_index1", "uint16_t", index1, 16))
    lines.append("")

    # ── decomposition trie level 2 ───────────────────────────────────────────
    lines.append("// Level-2 decomposition trie. Value = index into decomp_pool (0 = none).")
    lines.append(format_array("decomp_index2", "uint16_t", index2, 16))
    lines.append("")

    # ── composition pairs ────────────────────────────────────────────────────
    # Flatten to a single uint32_t array: [a0, b0, c0, a1, b1, c1, ...]
    flat_comp = []
    for a, b, c in comp_pairs:
        flat_comp.extend([a, b, c])
    lines.append("// Composition pair table: triples of (starter, combining, composed).")
    lines.append("// Sorted by (starter, combining) for binary search.")
    lines.append("// Hangul composition (U+AC00..U+D7A3) handled algorithmically.")
    lines.append(format_array("comp_pairs", "uint32_t", flat_comp, 6))
    lines.append("")

    # ── case fold table ──────────────────────────────────────────────────────
    # Flatten: [from0, n0, to0[0], to0[1], to0[2], from1, n1, to1[0], ...]
    flat_lower = []
    for frm, tos in lower_entries:
        padded = (tos + [0, 0, 0])[:3]
        flat_lower.extend([frm, len(tos)] + padded)
    lines.append("// Case fold table: quintuples of (from, n, to[0], to[1], to[2]).")
    lines.append("// Implements Unicode full case folding (C+F folds, no Turkish T folds).")
    lines.append("// Sorted by from for binary search.")
    lines.append(format_array("case_fold_pairs", "uint32_t", flat_lower, 5))
    lines.append("")

    flat_upper = []
    for frm, tos in upper_entries:
        padded = (tos + [0, 0, 0])[:3]
        flat_upper.extend([frm, len(tos)] + padded)
    lines.append("// Simple uppercase table: quintuples of (from, n=1, to[0], 0, 0).")
    lines.append("// Only 1:1 mappings (ignores rare multi-char upper results).")
    lines.append("// Sorted by from for binary search.")
    lines.append(format_array("upper_pairs", "uint32_t", flat_upper, 5))
    lines.append("")

    lines.append("} // namespace system")
    lines.append("} // namespace libbitcoin")
    lines.append("")
    return "\n".join(lines)

def generate_hpp(pool, index1, index2, comp_pairs, lower_entries, upper_entries):
    """Generate the .hpp header file."""
    lines = [LICENSE_HEADER]
    lines.append("#ifndef LIBBITCOIN_SYSTEM_UNICODE_UNICODE_TABLES_HPP")
    lines.append("#define LIBBITCOIN_SYSTEM_UNICODE_UNICODE_TABLES_HPP")
    lines.append("")
    lines.append("#include <cstddef>")
    lines.append("#include <cstdint>")
    lines.append("")
    lines.append(f"// Generated by tools/generate_unicode_tables.py")
    lines.append(f"// Unicode version: {unicodedata.unidata_version}")
    lines.append("")
    lines.append("namespace libbitcoin {")
    lines.append("namespace system {")
    lines.append("")
    lines.append(f"constexpr int decomp_shift = {SHIFT};")
    lines.append("")
    lines.append(f"constexpr size_t decomp_pool_count = {len(pool)};")
    lines.append(f"constexpr size_t decomp_index1_count = {len(index1)};")
    lines.append(f"constexpr size_t decomp_index2_count = {len(index2)};")
    lines.append(f"constexpr size_t comp_pairs_count = {len(comp_pairs)};")
    lines.append(f"constexpr size_t case_fold_pairs_count = {len(lower_entries)};")
    lines.append(f"constexpr size_t upper_pairs_count = {len(upper_entries)};")
    lines.append("")
    lines.append("extern const uint32_t decomp_pool[decomp_pool_count];")
    lines.append("extern const uint16_t decomp_index1[decomp_index1_count];")
    lines.append("extern const uint16_t decomp_index2[decomp_index2_count];")
    lines.append("extern const uint32_t comp_pairs[comp_pairs_count * 3];")
    lines.append("extern const uint32_t case_fold_pairs[case_fold_pairs_count * 5];")
    lines.append("extern const uint32_t upper_pairs[upper_pairs_count * 5];")
    lines.append("")
    lines.append("} // namespace system")
    lines.append("} // namespace libbitcoin")
    lines.append("")
    lines.append("#endif")
    lines.append("")
    return "\n".join(lines)

# ── main ──────────────────────────────────────────────────────────────────────

def main():
    print(f"Unicode version: {unicodedata.unidata_version}", file=sys.stderr)

    print("Building decomposition tables...", file=sys.stderr)
    pool, index1, index2 = build_decomp_tables()
    print(f"  pool: {len(pool)} uint32_t ({len(pool)*4//1024}KB)", file=sys.stderr)
    print(f"  index1: {len(index1)} uint16_t", file=sys.stderr)
    print(f"  index2: {len(index2)} uint16_t", file=sys.stderr)

    print("Building composition table...", file=sys.stderr)
    comp_pairs = build_comp_pairs()
    print(f"  pairs: {len(comp_pairs)}", file=sys.stderr)

    print("Building case fold tables...", file=sys.stderr)
    lower_entries, upper_entries = build_case_fold_table()
    print(f"  lower: {len(lower_entries)} entries", file=sys.stderr)
    print(f"  upper: {len(upper_entries)} entries", file=sys.stderr)

    cpp_path = "src/unicode/unicode_tables.cpp"
    hpp_path = "include/bitcoin/system/unicode/unicode_tables.hpp"

    with open(cpp_path, "w") as f:
        f.write(generate_cpp(pool, index1, index2, comp_pairs, lower_entries, upper_entries))
    print(f"Wrote {cpp_path}", file=sys.stderr)

    with open(hpp_path, "w") as f:
        f.write(generate_hpp(pool, index1, index2, comp_pairs, lower_entries, upper_entries))
    print(f"Wrote {hpp_path}", file=sys.stderr)

if __name__ == "__main__":
    main()
