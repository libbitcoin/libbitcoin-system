/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_KERNELS_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_KERNELS_HPP

#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

/// Arguments of the verify_ecdsa kernel, columns in device memory.
struct ecdsa_arguments
{
    const hash_digest* digests;
    const ec_compressed* keys;
    const ec_signature* signatures;
    uint8_t* results;
    uint32_t count;
};

/// Arguments of the verify_schnorr kernel, columns in device memory.
struct schnorr_arguments
{
    const hash_digest* messages;
    const ec_xonly* keys;
    const ec_signature* signatures;
    uint8_t* results;
    uint32_t count;
};

/// Maximum label keys of a scan_silent launch.
constexpr size_t maximum_labels = 8;

/// Output key prefix (first eight bytes of x).
using prefix = data_array<8>;

/// Arguments of the scan_silent kernel, columns in device memory. Each
/// summary has one prefix of the receiver output key, then one of that key
/// plus each label key, and is valid if its keys are computed.
struct silent_arguments
{
    const ec_compressed* summaries;
    prefix* prefixes;
    uint8_t* valid;
    ec_secret scan;
    ec_uncompressed spend;
    std_array<ec_uncompressed, maximum_labels> labels;
    uint32_t label_count;
    uint32_t count;
};

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
