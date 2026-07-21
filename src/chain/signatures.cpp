/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/batch/signatures.hpp>

#include <mutex>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Thread registry.
// ----------------------------------------------------------------------------
// Registration is append-only: pool threads live until process terminate, and
// purges stop with the validation chaser before that, so entries never dangle.

struct signature_registry
{
    std::mutex mutex{};
    std::vector<ecdsa_signatures*> ecdsa{};
    std::vector<schnorr_signatures*> schnorr{};
};

static signature_registry& registry() NOEXCEPT
{
    static signature_registry self{};
    return self;
}

// static
ecdsa_signatures& signatures::ecdsa_rows() NOEXCEPT
{
    static thread_local ecdsa_signatures rows{};
    [[maybe_unused]] static thread_local const auto once = [&]() NOEXCEPT
    {
        auto& self = registry();
        const std::unique_lock lock{ self.mutex };
        self.ecdsa.push_back(&rows);
        return true;
    }();

    return rows;
}

// static
schnorr_signatures& signatures::schnorr_rows() NOEXCEPT
{
    static thread_local schnorr_signatures rows{};
    [[maybe_unused]] static thread_local const auto once = [&]() NOEXCEPT
    {
        auto& self = registry();
        const std::unique_lock lock{ self.mutex };
        self.schnorr.push_back(&rows);
        return true;
    }();

    return rows;
}

// static
void signatures::purge() NOEXCEPT
{
    auto& self = registry();
    const std::unique_lock lock{ self.mutex };
    for (const auto rows: self.ecdsa)
        rows->purge();

    for (const auto rows: self.schnorr)
        rows->purge();
}

// Captures.
// ----------------------------------------------------------------------------
// Called only when enabled, on the (single) thread connecting the block.

bool signatures::ecdsa(const hash_digest& digest, const ec_compressed& key,
    const ec_signature& signature) const NOEXCEPT
{
    BC_ASSERT(enabled);
    return ecdsa_rows().append(digest, key, signature);
}

bool signatures::schnorr(const hash_digest& digest, const ec_xonly& point,
    const ec_signature& signature) const NOEXCEPT
{
    BC_ASSERT(enabled);
    schnorr_rows().append(digest, point, signature);
    return true;
}

bool signatures::multisig(const hash_digest& digest,
    std::span<const ec_compressed> keys,
    std::span<const ec_signature> sigs) const NOEXCEPT
{
    BC_ASSERT(enabled);
    return ecdsa_rows().append(digest, keys, sigs);
}

chain::threshold::cursor signatures::threshold(size_t rows) const NOEXCEPT
{
    BC_ASSERT(enabled);
    schnorr_rows().count_threshold(rows);
    return
    {
        .put = [](const hash_digest& digest, const ec_xonly& point,
            const ec_signature& signature) NOEXCEPT
        {
            schnorr_rows().append(digest, point, signature);
            return true;
        },
        .rows = rows
    };
}

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
