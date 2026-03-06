# UTF-8 Todos & Test Recommendations

Two distinct concerns:

1. **UTF-8 Everywhere** — make Windows behave like Linux/Mac at the char/string
   level. No ICU required; just UTF-8 ↔ UTF-16 conversion, console stream
   buffers, and wrappers around path/file APIs.

2. **Unicode normalization** — required for BIP-39/Electrum/BIP-38. Needs
   upper/lower case folding and the Electrum-specific CJK/diacritic functions.

---

## Concern 1: UTF-8 Everywhere

Implementation is largely complete. Scope: narrow. Todos below are verification
and documentation items.

### TODO 1 — Resolve the `std::filesystem::path` imbuing question

`environment.cpp` contains:

```cpp
// TODO: verify std::filesystem::path is inbued as it was with boost.
```

On Windows, `std::filesystem::path` uses `wchar_t` internally, so imbuing the
global locale with UTF-8 likely has no effect on path operations — they probably
already work correctly via `to_path()`/`from_path()` without needing the locale.
Verify and either remove the TODO with an explanatory comment or fix the imbuing.

**Recommended tests:**
- Construct a `std::filesystem::path` from a non-ASCII UTF-8 string via
  `to_path()`, round-trip through `from_path()`, verify identity.
- Windows-specific; should be included in a CI Windows matrix job.

---

### TODO 2 — Document `extended_path()` thread-safety limitation at the API level

`extended_path()` calls `GetFullPathNameW()` which uses process-global state on
Windows. Not thread-safe. Currently only documented inline in the implementation.
The limitation should be visible at the declaration in the header.

**Recommended tests:** None — platform limitation, documentation only.

---

### TODO 3 — Verify `utf8_remainder_size()` handles malformed UTF-8 safely

The function returns 0 on malformed input ("assumes no truncation"). Verify this
is the correct fallback in the stream context and that malformed sequences cannot
cause a read overrun or infinite loop inside `unicode_streambuf`.

**Recommended tests:**
- Feed known-malformed UTF-8 sequences into the `unicode_streambuf` output path:
  - Truncated 2-, 3-, and 4-byte sequences
  - Overlong encodings
  - Surrogate halves (U+D800–U+DFFF)
- Verify no crash, no data corruption, and graceful continuation.

---

### TODO 4 — Verify console stream round-trip for non-ASCII on Windows

The `console_streambuf` patches `std::wcin` with `ReadConsoleW()`.
`unicode_streambuf` handles the UTF-8 ↔ UTF-16 translation layer. Confirm that
a write/read round-trip is lossless for all non-ASCII input.

**Recommended tests:**
- Explicit round-trip tests for the full stack
  (unicode_streambuf → console_streambuf → ReadConsoleW) if not already present.
- Specifically cover characters that expand in UTF-8 (≥ U+0800) and astral-plane
  characters encoded as surrogate pairs in UTF-16 (U+10000–U+10FFFF).

---

## Concern 2: Unicode Normalization (BIP-39 / Electrum / BIP-38)

Normalization pipeline is implemented and all known test vectors pass.
Todos below are correctness verification, cleanup, and coverage gaps.

### TODO 5 — Remove stale ICU TODO in `electrum.cpp`

Around line 172, `electrum.cpp` has a comment:

```
// replace the ICU lib dependency with Python's unicodedata
```

This is done (ICU removed, embedded tables in place). Remove the comment.

**Recommended tests:** None — documentation cleanup only.

---

### TODO 6 — Audit all callers of `to_non_combining_form()` — must be post-decomposition

`to_non_combining_form()` removes code points with CCC > 0. Called on precomposed
text, precomposed characters like `é` (U+00E9, CCC = 0) pass through unchanged —
the combining accent is only visible after NFKD decomposition. Electrum does this
correctly (NFKD first, then remove combining). Verify no other caller skips the
decomposition step.

**Recommended tests:**
- `to_non_combining_form("é")` (precomposed U+00E9) → `"é"` (unchanged, CCC = 0)
- `to_non_combining_form(to_compatibility_decomposition("é"))` → `"e"` (accent removed)
- Add both cases explicitly if not already present.

---

### TODO 7 — Verify `to_compressed_form()` matches Electrum Python exactly

The function compresses ASCII whitespace and removes spaces between CJK
characters. Verify against the reference Python `normalize_text()` for all
whitespace variants and ideographic space (U+3000) behavior.

**Recommended tests:**
- Multiple spaces between non-CJK → single space
- Single space between CJK → removed
- Ideographic space (U+3000) between CJK → preserved
- Tab (U+0009), LF (U+000A), VT (U+000B), FF (U+000C), CR (U+000D) → single space
- Mixed CJK/non-CJK sequences

Compare output byte-for-byte with Python reference for each case.

---

### TODO 8 — Verify callers handle multi-character case expansion

`to_lower()` and `to_upper()` can expand strings (e.g., `ß` → `ss`, `DŽ` →
`dž`). The public API returns `bool`. Confirm all callers do not assume the
output string is the same length as the input.

**Recommended tests:**
- `to_lower("ß")` → `"ss"` (length 1 → 2)
- `to_upper("ß")` → `"SS"` (length 1 → 2)
- `to_lower("DŽ")` → `"dž"` (U+01C5, multi-char)
- Verify no caller performs `output[i] = to_lower(input[i])` style indexing.

---

### TODO 9 — Add Unicode Consortium conformance vectors

The normalization tests use BIP-38/39/Electrum known-answer vectors plus a
selection of hand-written Unicode cases. The Unicode Consortium publishes
`NormalizationTest.txt` (Unicode 15.0.0) with exhaustive conformance vectors
for NFC, NFD, NFKC, NFKD. Adding a subset would give stronger algorithmic
guarantees independent of wallet usage.

**Recommended tests:**
- Fetch `NormalizationTest.txt` (Unicode 15.0.0 — matches embedded tables).
- Add at minimum the "Part 1" general coverage vectors to
  `test/unicode/normalization.cpp`.
- These vectors include edge cases not covered by BIP test vectors (e.g.,
  compatibility mappings for mathematical/enclosed forms, rare combining classes).

---

### TODO 10 — Verify Hangul edge cases beyond the composition bug fix

The starter-to-starter composition fix (L+V → LV, LV+T → LVT) was validated.
Verify the boundary conditions:

**Recommended tests:**
- Isolated L jamo alone → not composed
- Isolated V jamo alone → not composed
- Isolated T jamo alone → not composed
- T without preceding LV → T left unchanged
- Already-composed LVT syllable → round-trips correctly through NFD → NFC
- LV syllable (no T) → round-trips correctly through NFD → NFC
- L + V + T sequence → composes to LVT in one NFC pass

---

### TODO 11 — Verify BIP-38 NFC with a decomposed-input passphrase

The existing BIP-38 test uses `ΜΟΛΩΝ ΛΑΒΕ` (Greek, already in NFC). Add a
vector where the passphrase is in NFD (decomposed) form and verify:
1. NFC output matches the precomposed form byte-for-byte.
2. The resulting encrypted key matches the BIP-38 reference for that passphrase.

**Recommended tests:**
- Passphrase `"e\u0301"` (e + combining acute accent, NFD) → NFC → `"é"` (U+00E9)
- Full BIP-38 encrypt/decrypt round-trip with NFD input passphrase, verify key
  matches result of NFC input passphrase.
