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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SCAN_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SCAN_HPP

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {

namespace scan {

/// Spans match serialized columns, one element for each transaction, with
/// the output key prefixes of all transactions in one column, which each
/// transaction's outputs index.
struct BC_API batch
{
    using prefix = data_array<8>;
    using tx_link = data_array<4>;
    using tx_link_t = unsigned_type<sizeof(tx_link)>;
    using prefix_link = data_array<4>;
    using prefix_count = data_array<3>;

#pragma pack(push, 1)
    struct outputs_t
    {
        prefix_link first;
        prefix_count count;
    };
#pragma pack(pop)
    using handler = std::function<void(const code&, tx_link_t,
        const ec_compressed&)>;

    /// Scan secret, spend key and label keys of a receiver.
    struct receiver
    {
        ec_secret scan{};
        ec_uncompressed spend{};
        ec_uncompresseds labels{};
    };

    std::span<const tx_link> correlates;
    std::span<const ec_compressed> points;
    std::span<const outputs_t> outputs;
    std::span<const prefix> prefixes;

    /// Invoke callback for each transaction with an output paying receiver.
    static void scan(const stopper& cancel, const batch& batch,
        const receiver& keys, const handler& callback, bool turbo) NOEXCEPT;

protected:
    /// Transactions scanned by one task.
    static constexpr size_t chunk_txs = power2(10_size);

    /// Scans of at least this many transactions run on the device where
    /// available.
    static constexpr size_t device_txs = power2(16_size);

    /// Transactions sent to the device at once.
    static constexpr size_t device_groups = power2(22_size);

    /// Scan on the device, false if failed with resume the first unscanned
    /// transaction.
    static bool scan_device(size_t& resume, const stopper& cancel,
        const batch& batch, const receiver& keys, const handler& callback,
        bool turbo) NOEXCEPT;
};

} // namespace scan
} // namespace system
} // namespace libbitcoin

#endif
