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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SCHNORR_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SCHNORR_HPP

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/batch/verify.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {

namespace schnorr {

/// Spans match serialized buffers, a row for each correlate.
struct BC_API batch
{
#pragma pack(push, 1)
    struct correlate_t
    {
        batched::link id;
    };

    struct row_t
    {
        hash_digest digest;
        ec_xonly point;
        ec_signature signature;
    };
#pragma pack(pop)

    static batched::links_t verify(const stopper& cancel,
        const batch& batch) NOEXCEPT;

    std::span<const correlate_t> correlates;
    std::span<const row_t> rows;

protected:
    static batched::links_t get_failures(const stopper& cancel,
        const data_chunk& out, const batch& in) NOEXCEPT;
    static data_chunk evaluate(const stopper& cancel,
        const batch& batch) NOEXCEPT;
    static batched::links_t correlate(const stopper& cancel,
        const data_chunk& out, const batch& batch) NOEXCEPT;
};

} // namespace schnorr
} // namespace system
} // namespace libbitcoin

#endif
