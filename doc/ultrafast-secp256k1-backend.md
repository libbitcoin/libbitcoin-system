# UltrafastSecp256k1 secp256k1 backend (GPU/CPU batch signature verification)

This is a **proposal / work-in-progress** integration that lets libbitcoin-system
use [UltrafastSecp256k1](https://github.com/shrec/UltrafastSecp256k1) as its
secp256k1 backend. It provides two things:

1. **Drop-in libsecp256k1 ABI** — through the engine's compatibility shim, so the
   existing `bitcoin/system/crypto/secp256k1.*` wrappers compile and link
   **unchanged**. Single-signature behaviour is libsecp256k1-compatible.
2. **Batch script-signature verification** — `ufsecp_lbtc_verify_ecdsa` /
   `ufsecp_lbtc_verify_schnorr`: hand a node the `sig / key / sighash` triples
   extracted from script execution as one array and get an array of per-row
   pass/fail results back, GPU-accelerated with a mandatory CPU fallback. This
   targets IBD / historical block validation (signatures are ~95% of script
   cost), **not** mempool latency. A BIP-352 silent-payment scan entry point
   (`ufsecp_lbtc_sp_scan`) is also exposed.

> Status: build wiring + entry-point contract. The batch verification API is a
> proposal agreed in principle; the IBD marshalling and the GPU-vs-CPU
> consensus differential gate are to be built and tuned together.

## Build

```bash
git submodule update --init --recursive deps/UltrafastSecp256k1

cmake -S builds/cmake -B build \
    -Dwith-ultrafast=ON \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

- `-Dwith-ultrafast=ON` replaces `find_package(libsecp256k1)` with the engine
  (built in its minimal `SECP256K1_BUILD_LIBBITCOIN` profile: shim + CT + strict
  ABI, everything else off) and aliases `libsecp256k1::secp256k1` to the shim.
- Engine location override: `-DUltrafastSecp256k1_DIR=/path/to/UltrafastSecp256k1`.
- GPU acceleration: add a GPU backend to the engine configure, e.g.
  `-DSECP256K1_BUILD_CUDA=ON` (or `-DSECP256K1_BUILD_OPENCL=ON`). Without one the
  batch path runs on the CPU reference (still correct, just not accelerated).

Default (`with-ultrafast=OFF`) keeps the stock `find_package(libsecp256k1 0.7.0)`
behaviour — zero impact on existing builds.

## Batch verification entry point

Header: `ufsecp_libbitcoin.h` (from the engine; pure C / C++, no FFI). Link the
engine C ABI library `ufsecp_static`.

Records are packed into one unified table; ECDSA and Schnorr are **separate
calls** (uniform data, stacked independently):

| Kind    | Record (verified)                | Bytes |
|---------|----------------------------------|-------|
| ECDSA   | `32 msg ‖ 33 pubkey ‖ 64 sig`    | 129   |
| Schnorr | `32 xonly ‖ 32 msg ‖ 64 sig`     | 128   |

Each row may carry a trailing **opaque correlation key** (`key_size` bytes, e.g.
a 3-byte block id or 4-byte tx id; `key_size = 0` disables it). The bridge never
interprets it — it is carried so an invalid row maps back to its block/tx.

```cpp
#include <ufsecp_libbitcoin.h>

ufsecp_lbtc_ctrl* ctrl = nullptr;
ufsecp_lbtc_ctrl_create(&ctrl, UFSECP_LBTC_AUTO);   // GPU if available, else CPU

// rows: n records of (129 + key_size) bytes, built from script sig/key/sighash.
std::vector<uint8_t> results(n);
std::vector<size_t>  invalid(n);
size_t n_invalid = 0;

ufsecp_lbtc_verify_ecdsa(ctrl, rows.data(), n, key_size,
                         results.data(),            // per-row 1=valid / 0=invalid
                         invalid.data(), n, &n_invalid);  // failing row indices

if (n_invalid != 0) {
    // block invalid — map invalid[i] back to its tx via the row's opaque key
}
ufsecp_lbtc_ctrl_destroy(ctrl);
```

`ufsecp_lbtc_verify_schnorr` is identical with the 128-byte Schnorr record.

## Consensus

The GPU path is consensus-bearing: its result must match the CPU /
libsecp256k1-equivalent reference bit-for-bit (enforced by a differential gate on
the engine side). The CPU fallback is always present and never optional.

## What is here vs. what comes next

- **Here:** build option + backend wiring + the batch entry-point contract and
  usage. Single-signature verification works as a libsecp256k1 drop-in today.
- **Next (to build/tune together):** marshalling the per-block signature jobs
  into the batch arrays inside the IBD flow, the GPU-vs-CPU consensus
  differential gate, and GPU throughput tuning on the real pipeline.
