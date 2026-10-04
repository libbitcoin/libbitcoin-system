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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_LAYOUT_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_LAYOUT_HPP

#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include "kernels.hpp"

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

struct shape
  : algorithm
{
    static constexpr auto slice_bytes = table_words * sizeof(uint64_t);
    static constexpr auto table_bytes = slice_count * slice_bytes;
    static constexpr auto part_size = comb_part_windows * comb_size;
    static constexpr auto part_bytes = part_size * comb_words * sizeof(uint64_t);
    static constexpr auto comb_bytes = comb_part_count * part_bytes;
};

/// Columns of a kernel's rows within the staging buffer.
template <typename Key>
struct layout
{
    using arguments = iif<is_same_type<Key, ec_compressed>, ecdsa_arguments,
        schnorr_arguments>;

    static constexpr size_t align = 256;
    static constexpr size_t row_bytes = sizeof(hash_digest) + sizeof(Key) +
        sizeof(ec_signature) + one;

    static constexpr size_t round(size_t bytes) NOEXCEPT
    {
        return ceilinged_divide(bytes, align) * align;
    }

    /// Rows of the kernel that fit in the given bytes.
    static constexpr size_t rows(size_t bytes) NOEXCEPT
    {
        constexpr auto slack = 3 * align;
        return bytes < slack ? zero : (bytes - slack) / row_bytes;
    }

    explicit constexpr layout(size_t rows) NOEXCEPT
      : keys(round(rows * sizeof(hash_digest))),
        signatures(round(keys + rows * sizeof(Key))),
        results(round(signatures + rows * sizeof(ec_signature)))
    {
    }

    const size_t keys;
    const size_t signatures;
    const size_t results;
};

/// Columns of scan rows within the staging buffer, stride prefixes per row.
struct silent_layout
{
    static constexpr size_t align = 256;

    static constexpr size_t row_bytes(size_t stride) NOEXCEPT
    {
        return sizeof(ec_compressed) + stride * sizeof(prefix) + one;
    }

    static constexpr size_t round(size_t bytes) NOEXCEPT
    {
        return ceilinged_divide(bytes, align) * align;
    }

    /// Rows that fit in the given bytes.
    static constexpr size_t rows(size_t bytes, size_t stride) NOEXCEPT
    {
        constexpr auto slack = 2 * align;
        return bytes < slack ? zero : (bytes - slack) / row_bytes(stride);
    }

    constexpr silent_layout(size_t rows, size_t stride) NOEXCEPT
      : prefixes(round(rows * sizeof(ec_compressed))),
        valid(round(prefixes + rows * stride * sizeof(prefix)))
    {
    }

    const size_t prefixes;
    const size_t valid;
};

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
