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
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// protected
// ----------------------------------------------------------------------------

size_t silent::batch::next(const batch& batch, size_t row) NOEXCEPT
{
    const auto& correlate = batch.correlates[row];
    const auto count = batch.correlates.size();
    do { ++row; } while (row < count && batch.correlates[row] == correlate);
    return row;
}

bool silent::batch::is_match(const batch& batch, size_t first, size_t last,
    const receiver& keys) NOEXCEPT
{
    auto shared = batch.points[first];
    if (!ec_multiply(shared, keys.scan))
        return false;

    accumulator<sha256> hasher{ tagged_midstate<"BIP0352/SharedSecret">, one };
    hasher.write(shared);
    hasher.write(to_big_endian(0_u32));
    const auto tweak = hasher.flush();

    auto key = keys.spend;
    if (!ec_add(key, tweak))
        return false;

    constexpr auto size = array_count<prefix>;
    const auto prefixes = batch.prefixes.subspan(first, last - first);
    const auto paid = [&](const ec_uncompressed& point) NOEXCEPT
    {
        return contains(prefixes, array_cast<uint8_t, size, one>(point));
    };

    if (paid(key))
        return true;

    ec_uncompressed labeled{};
    for (const auto& label: keys.labels)
    {
        labeled = key;
        if (ec_add(labeled, label) && paid(labeled))
            return true;
    }

    return false;
}

// scan
// ----------------------------------------------------------------------------
// The callback provides granular response when the query is very long-running,
// which is consistent with the push notification public interface.

void silent::batch::scan(const stopper& cancel, const batch& batch,
    const receiver& keys, const handler& callback, bool turbo) NOEXCEPT
{
    const auto policy = poolstl::execution::par_if(turbo);

    // Three spans are corresponding arrays of equal length.
    const auto count = batch.correlates.size();
    BC_ASSERT(batch.prefixes.size() == count);
    BC_ASSERT(batch.points.size() == count);

    std_vector<size_t> chunks(ceilinged_divide(count, chunk_rows));
    std::iota(chunks.begin(), chunks.end(), zero);

    // A chunk owns the transactions that begin within it.
    // Return from scan with !cancel implies complete.
    std::for_each(policy, chunks.cbegin(), chunks.cend(),
        [&](size_t chunk) NOEXCEPT
        {
            auto row = chunk * chunk_rows;
            const auto end = std::min(row + chunk_rows, count);
            if (is_nonzero(row) &&
                batch.correlates[row] == batch.correlates[sub1(row)])
                row = next(batch, row);

            for (auto last = row; row < end && !cancel; row = last)
            {
                last = next(batch, row);
                if (is_match(batch, row, last, keys))
                {
                    const auto link = from_little_endian(batch.correlates[row]);
                    callback({}, link, batch.points[row]);
                }
            }
        });
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
