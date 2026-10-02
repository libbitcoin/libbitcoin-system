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

/// Span matches serialized buffer.
struct BC_API batch
{
    using prefix = data_array<8>;
    using tx_link = data_array<4>;
    using tx_link_t = unsigned_type<sizeof(tx_link)>;
    ////using tx_links_t = std::vector<tx_link_t>;
    using handler = std::function<void(const code&, tx_link_t)>;

    std::span<const tx_link> correlates;
    std::span<const prefix> prefixes;
    std::span<const ec_compressed> points;

    static void scan(const stopper& cancel, const batch& batch,
        const ec_secret& scan_key, const handler& callback,
        bool turbo) NOEXCEPT;

protected:
    static bool get_match(tx_link_t& out, const batch& batch,
        size_t row, const ec_secret& scan_key) NOEXCEPT;
};

} // namespace silent
} // namespace system
} // namespace libbitcoin

#endif
