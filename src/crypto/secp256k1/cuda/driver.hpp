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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_DRIVER_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_DRIVER_HPP

#include <bitcoin/system/crypto/secp256k1/batch/ecdsa.hpp>
#include <bitcoin/system/crypto/secp256k1/batch/schnorr.hpp>
#include <bitcoin/system/crypto/secp256k1/batch/scan.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include "kernels.hpp"

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

/// True if gpu verification is compiled in.
bool compiled() NOEXCEPT;

/// True if gpu is loaded and has not failed.
bool available() NOEXCEPT;

/// Verify rows on gpu, one result byte per row, empty if all rows are valid
/// or if canceled. False if gpu failed, which disables it.
bool verify(const stopper& cancel, data_chunk& out,
    const ecdsa::batch& batch) NOEXCEPT;
bool verify(const stopper& cancel, data_chunk& out,
    const schnorr::batch& batch) NOEXCEPT;

/// Output key prefixes of receiver for each point on gpu, one
/// then one per label key (at most maximum_labels), with valid for each
/// point computed. False if gpu failed, which disables it, or if
/// there are too many labels.
bool scan(const stopper& cancel, std::vector<prefix>& prefixes,
    data_chunk& valid, const std::span<const ec_compressed>& points,
    const scan::batch::receiver& keys) NOEXCEPT;

/// Point (hash * sum) of each row on gpu, with valid for each row
/// computed, into spans sized to rows. False if gpu failed,
/// which disables it.
bool compute(const stopper& cancel, const std::span<ec_compressed>& out,
    const std::span<uint8_t>& valid,
    const std::span<const ec_compressed>& sums,
    const std::span<const ec_secret>& hashes) NOEXCEPT;

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
