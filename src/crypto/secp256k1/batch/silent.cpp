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
#include <bitcoin/system/crypto/secp256k1/batch/silent.hpp>

#include <numeric>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/execution.hpp>

namespace libbitcoin {
namespace system {

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// get_match
// ----------------------------------------------------------------------------

bool silent::batch::get_match(tx_link_t& , const batch& , size_t ,
    const ec_secret& ) NOEXCEPT
{
    // TODO: implement.
    return false;
}

// scan
// ----------------------------------------------------------------------------
// The callback provides granular response when the query is very long-running,
// which is consistent with the push notification public interface.

void silent::batch::scan(const stopper& cancel, const batch& batch,
    const ec_secret& scan_key, const handler& callback, bool turbo) NOEXCEPT
{
    const auto policy = poolstl::execution::par_if(turbo);

    // Three spans are corresponding arrays of equal length.
    const auto count = batch.correlates.size();
    BC_ASSERT(batch.prefixes.size() == count);
    BC_ASSERT(batch.points.size() == count);

    std::vector<size_t> it(count);
    std::iota(it.begin(), it.end(), zero);

    // Return from scan with !cancel implies complete.
    std::for_each(policy, it.cbegin(), it.cend(), [&](size_t row) NOEXCEPT
    {
        if (cancel)
            return;

        tx_link_t tx{};
        if (get_match(tx, batch, row, scan_key))
            callback({}, tx);
    });
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
