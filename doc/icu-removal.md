# ICU Removal — Design Notes

## Background

libbitcoin-system previously required ICU (International Components for Unicode)
for Unicode normalization and case folding. This was accessed indirectly through
Boost.Locale's ICU backend, which was invoked in three wallet operations:

| Operation | Form | Caller |
|---|---|---|
| BIP-38 passphrase encryption/decryption | NFC | `encrypted_keys.cpp` |
| BIP-39 mnemonic seed derivation | NFKD | `mnemonic.cpp` |
| Electrum seed derivation | NFKD + to_lower | `electrum.cpp` |

Without ICU, all six normalization functions (`to_lower`, `to_upper`, NFC, NFD,
NFKC, NFKD) explicitly returned `false`. Callers checked this return value and
propagated failure, so non-ASCII passphrases were rejected with an error rather
than silently corrupted. ASCII-only usage worked correctly without ICU.

### The Boost build hack

Because Boost.Locale's ICU detection uses a static library link test, it fails
when Boost is built against a custom prefix (as libbitcoin does). The shell-based
install script worked around this with `circumvent_boost_icu_detection()`, which
patched Boost's own `has_icu_test.cpp` files on disk before the b2 build:

```bash
# install-cmakepresets.sh (removed)
circumvent_boost_icu_detection()
{
    # Replace Boost's ICU test with one that always succeeds.
    ...
    find "$BOOST_SRC" -name "has_icu_test.cpp" -exec cp "$TEMP" {} \;
}
```

This hack cannot be reproduced in a CMake `ExternalProject_Add` build. It was
the primary reason the CMake-based dependency build could not use ICU, which in
turn prevented normalization from working when dependencies were built via CMake.

---

## Solution: embedded Unicode tables

The codebase already contained CPython's two-level trie tables
(`unicode_data1`, `unicode_data2`, `combining_index` in `code_points.cpp`) used
for combining-class lookups. A `TODO` comment in `electrum.cpp:172` explicitly
noted the intent: *"replace the ICU lib dependency with Python's unicodedata."*

This change completes that work. All normalization is now implemented in pure C++
using tables generated directly from Python's `unicodedata` module — the same
Unicode data source CPython itself uses.

---

## What changed

### New files

**`tools/generate_unicode_tables.py`**

Python script that generates the C++ table files from `unicodedata`. Run from
the repository root:

```
python3 tools/generate_unicode_tables.py
```

The generated files carry a version comment identifying both the Unicode standard
version and the Python version used:

```
// Unicode version: 15.0.0 (Python 3.12.3)
// To update: install a Python version with the desired Unicode data and re-run.
```

Python ships a specific Unicode version with each release (see
[python.org/downloads](https://www.python.org/downloads/release/)). Upgrading
Unicode data means installing a newer Python and re-running the script — the
generated `.cpp` and `.hpp` are then checked in as usual.

**`src/unicode/unicode_tables.cpp`** (~6 200 lines, generated)

Six tables:

| Table | Type | Size | Purpose |
|---|---|---|---|
| `decomp_pool` | `uint32_t[]` | 10 320 entries | Pool of decomposed code point sequences |
| `decomp_index1` | `uint16_t[]` | 8 704 entries | Level-1 decomposition trie (cp >> 7) |
| `decomp_index2` | `uint16_t[]` | 13 568 entries | Level-2 decomposition trie → pool index |
| `comp_pairs` | `uint32_t[]` | 941 × 3 entries | Canonical composition pairs (a, b, composed) |
| `case_fold_pairs` | `uint32_t[]` | 1 530 × 5 entries | Case-fold sequences for `to_lower` |
| `upper_pairs` | `uint32_t[]` | 1 423 × 5 entries | Upper-case sequences for `to_upper` |

**`include/bitcoin/system/unicode/unicode_tables.hpp`** (generated)

Declarations for the above, plus the `decomp_shift = 7` constant shared with the
existing `unicode_data1/2` trie.

---

### Modified files

**`src/unicode/normalization.cpp`**

Completely rewritten. Removed:
- `#include <boost/locale.hpp>` usage for normalization
- `get_backend_manager()` and its ICU locale setup
- All `#ifdef HAVE_MSC` / `#else` / `#endif` blocks that selected between
  Windows-native and ICU paths
- All `#ifndef HAVE_ICU return false #endif` early-exit guards

Replaced with pure C++ helpers using the embedded tables:

| Helper | Purpose |
|---|---|
| `get_ccc(cp)` | Canonical combining class via existing `unicode_data1/2` + `combining_index` |
| `decompose_one(out, cp, compat)` | Recursive single-step decomposition; Hangul handled algorithmically |
| `canonical_order(seq)` | Stable-sort adjacent combining chars by CCC (UAX #15 canonical ordering) |
| `comp_lookup(a, b)` | Binary-search `comp_pairs`; Hangul L+V / LV+T handled algorithmically |
| `compose(seq)` | NFC composition pass (UAX #15 canonical composition algorithm) |
| `normal_form(value, compat, recompose)` | Orchestrates decompose → order → optionally compose |
| `fold_lower_one(cp, out)` | Binary-search `case_fold_pairs` → emit fold sequence |
| `fold_upper_one(cp, out)` | Binary-search `upper_pairs` → emit upper sequence |
| `apply_case(out, in, lower)` | Iterates code points, applies case fold, re-encodes UTF-8 |

The six public API functions (`to_lower`, `to_upper`, `to_canonical_composition`,
`to_canonical_decomposition`, `to_compatibility_composition`,
`to_compatibility_decomposition`) are unchanged in signature and semantics.

**`include/bitcoin/system/have.hpp`**

```cpp
// Before
/// Build configured (always available on msvc).
#if defined(HAVE_MSC) || defined(WITH_ICU)
    #define HAVE_ICU
#endif

// After
/// Unicode normalization is always available (embedded tables, no ICU needed).
#define HAVE_ICU
```

`HAVE_ICU` is now unconditional. All platforms including Windows always have
working normalization.

**`include/bitcoin/system/boost.hpp`**

Removed the `BOOST_HAS_ICU` preprocessor block that told Boost.Locale to use
ICU. Boost.Locale is still included (needed for `boost::locale::conv::utf_to_utf`
in `conversion.cpp` and `boost::locale::generator` in `environment.cpp`), but
it no longer needs an ICU backend.

**`builds/cmake/CMakeLists.txt`**

Removed:
- `option(with-icu ...)` cache variable
- `add_definitions(-DWITH_ICU)` and `${icu}` references
- `find_package(Icu-I18N 55.2 REQUIRED)` block
- All `icu_i18n_FOR_BUILD_*` variable sets and their use in include/library dirs
- `icu_i18n` from link targets

Added:
- `../../src/unicode/unicode_tables.cpp` to the library source list

**`install-cmakepresets.sh`**

Removed:
- `--with-icu` and `--build-icu` option handling
- `ICU_PREFIX`, `ICU_LIBS`, `ICU_URL`, `ICU_ARCHIVE`, `ICU_FLAGS`,
  `ICU_OPTIONS`, `BOOST_ICU_ICONV`, `BOOST_ICU_POSIX` variables
- `circumvent_boost_icu_detection()` function
- `initialize_boost_icu_configuration()` function
- `--with-icu=$ICU_PREFIX` argument to `./bootstrap.sh`
- `boost.locale.iconv`, `boost.locale.posix`, `-sICU_PATH` from the b2 invocation
- ICU download/build/unpack calls from `build_all()`

The b2 bootstrap is now simply:
```bash
./bootstrap.sh "--with-bjam=./b2" "--prefix=$PREFIX"
```

---

### Tests

All `#ifdef HAVE_ICU` / `#endif` conditional compilation guards were removed
from the test suite. Tests that previously only ran with ICU now always run:

| File | Change |
|---|---|
| `test/unicode/normalization.cpp` | Removed all ICU guards; added Hangul, canonical ordering, compatibility, BIP-38/39/Electrum conformance, and casing tests (82 test cases total) |
| `test/wallet/mnemonics/mnemonic.cpp` | Japanese BIP-39 vectors (24 vectors with non-ASCII passphrases) now always run |
| `test/wallet/mnemonics/electrum.cpp` | Non-ASCII passphrase vectors always run; removed ASCII-only fallback branch |
| `test/wallet/keys/encrypted_keys.cpp` | BIP-38 Greek passphrase (`ΜΟΛΩΝ ΛΑΒΕ`) tests always run |
| `test/words/catalogs/electrum.cpp` | Always uses ICU-correct abnormal-count values |
| `test/words/languages.cpp` | Removed "without ICU" non-normalizing test case |

Full test run: **4 174 tests, 0 failures**.

### Interactive demo

`examples/normalization_demo.cpp` is a standalone console program that
exercises all six normalization functions with annotated inputs and hex output.
It covers the same use cases as the test suite (BIP-38, BIP-39 Japanese,
Hangul, case folding) and is intended as a readable usage example.

Build with:

```bash
cmake -Dwith-normalization-demo=yes <other configure args>
cmake --build obj/<preset> --target libbitcoin-system-normalization-demo
./obj/<preset>/libbitcoin-system-normalization-demo
```

---

## What was NOT changed

- `src/unicode/utf8_everywhere/environment.cpp` — the existing
  `boost::locale::generator` call in `call_utf8_main()` is Windows-only
  (`#ifdef HAVE_MSC`) and does not require an ICU backend.
- `src/unicode/conversion.cpp` — uses `boost::locale::conv::utf_to_utf<>()`,
  which is a pure UTF encoding conversion (no locale, no ICU required).
- `src/unicode/code_points.cpp` / `code_points.hpp` — the existing
  `unicode_data1`, `unicode_data2`, `combining_index` tables are still used as-is
  for CCC lookups in `normalization.cpp`.
- The public normalization API in `normalization.hpp` — signatures and semantics
  are identical.
- Boost.Locale itself — still a build dependency; only its ICU backend is no
  longer required.

---

## Updating Unicode data

When a new Unicode standard version is available via a newer Python release:

1. Install the new Python version.
2. Run `python3 tools/generate_unicode_tables.py` from the repository root.
3. The script regenerates `src/unicode/unicode_tables.cpp` and
   `include/bitcoin/system/unicode/unicode_tables.hpp` with an updated version
   comment.
4. Commit the regenerated files. The normalization test suite validates
   correctness via BIP-38/39/Electrum known-answer vectors and Unicode
   conformance cases.

Python → Unicode version mapping (recent releases):

| Python | Unicode |
|---|---|
| 3.9 | 13.0.0 |
| 3.10 | 13.0.0 |
| 3.11 | 15.0.0 |
| 3.12 | 15.0.0 |
| 3.13 | 15.1.0 |
