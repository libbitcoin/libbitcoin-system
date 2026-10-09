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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SILENT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_SILENT_HPP

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {

namespace silent {

/// Spans match serialized buffers, a row for each correlate.
/// Rows of one transaction are contiguous and share one prevouts summary.
struct BC_API batch
{
    using prefix = data_array<8>;
    using tx_link = data_array<4>;
    using tx_link_t = unsigned_type<sizeof(tx_link)>;

#pragma pack(push, 1)
    struct row_t
    {
        batch::prefix prefix;
        ec_compressed point;
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
    std::span<const row_t> rows;

    /// Invoke callback for each transaction with an output paying receiver.
    static void scan(const stopper& cancel, const batch& batch,
        const receiver& keys, const handler& callback, bool turbo) NOEXCEPT;

protected:
    /// Rows scanned by one task.
    static constexpr size_t chunk_rows = power2(10_size);

    /// Scans of at least this many rows run on the device where available.
    static constexpr size_t device_rows = power2(16_size);

    /// Transactions sent to the device at once.
    static constexpr size_t device_groups = power2(22_size);

    /// The row that follows the transaction of the given row.
    static size_t next(const batch& batch, size_t row) NOEXCEPT;

    /// Scan on the device, false if failed with resume the first unscanned row.
    static bool scan_device(size_t& resume, const stopper& cancel,
        const batch& batch, const receiver& keys, const handler& callback,
        bool turbo) NOEXCEPT;
};

} // namespace silent
} // namespace system
} // namespace libbitcoin

#endif
