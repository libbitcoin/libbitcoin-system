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
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/execution.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include "batch.hpp"

namespace libbitcoin {
namespace system {

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// scan_rows
// ----------------------------------------------------------------------------

class silent_dispatcher
  : public secp256k1::algorithm
{
public:
    using affine = affine_t<uint64_t>;
    using affines = std_vector<affine>;
    using scalar = scalar_t;
    using scalars = std_vector<scalar_t>;
    using algorithm::from_bytes;
    using algorithm::is_zero_scalar;
    using algorithm::multiply;
    using algorithm::tweak;
};

// Receiver keys parsed once for all chunks of a scan.
struct parsed
{
    silent_dispatcher::scalar scan{};
    silent_dispatcher::affine spend{};
    silent_dispatcher::affines labels{};
};

static bool parse(parsed& out, const silent::batch::receiver& keys) NOEXCEPT
{
    if (!silent_dispatcher::from_bytes(out.scan, keys.scan) ||
        !silent_dispatcher::from_bytes(out.spend, keys.spend))
        return false;

    out.labels.resize(keys.labels.size());
    for (size_t index{}; index < keys.labels.size(); ++index)
    {
        auto& label = out.labels[index];
        if (!silent_dispatcher::from_bytes(label, keys.labels[index]))
            return false;
    }

    return true;
}

// The shared secret of each transaction (k = 0) tweaks the spend key, and each
// output key (and labeled key) is matched by prefix to the transaction rows.
static void scan_rows(const silent::batch& batch,
    const std::span<const size_t>& firsts,
    const std::span<const size_t>& lasts, const parsed& keys,
    const silent::batch::handler& callback) NOEXCEPT
{
    const auto count = firsts.size();
    std_vector<ec_compressed> points(count);
    for (size_t group{}; group < count; ++group)
        points[group] = batch.points[firsts[group]];

    data_chunk computed{};
    std_vector<ec_compressed> shared{};
    with_lanes([&]<typename Word>() NOEXCEPT
    {
        silent_dispatcher::multiply<Word>(computed, shared, points, keys.scan);
        return true;
    });

    // The tagged hash of each shared point and k = 0 [bip352].
    constexpr auto k = to_big_endian(0_u32);
    constexpr auto& midstate = tagged_midstate<"BIP0352/SharedSecret">;
    silent_dispatcher::scalars tweaks{};
    std_vector<size_t> groups{};
    tweaks.reserve(count);
    groups.reserve(count);
    for (size_t group{}; group < count; ++group)
    {
        if (is_zero(computed[group]))
            continue;

        const auto data = splice(shared[group], k);
        const auto hash = sha256::hash(midstate, data);

        silent_dispatcher::scalar tweak{};
        if (silent_dispatcher::from_bytes(tweak, hash) &&
            !silent_dispatcher::is_zero_scalar(tweak))
        {
            tweaks.push_back(tweak);
            groups.push_back(group);
        }
    }

    data_chunk tweaked{};
    std_vector<ec_xonly> outputs{};
    const auto& spend = keys.spend;
    const auto& labels = keys.labels;
    with_lanes([&]<typename Word>() NOEXCEPT
    {
        silent_dispatcher::tweak<Word>(tweaked, outputs, tweaks, spend, labels);
        return true;
    });

    constexpr auto size = array_count<silent::batch::prefix>;
    const auto stride = add1(keys.labels.size());
    const std::span<const ec_xonly> keyed{ outputs };
    for (size_t index{}; index < groups.size(); ++index)
    {
        if (is_zero(tweaked[index]))
            continue;

        const auto group = groups[index];
        const auto first = firsts[group];
        const auto rows = lasts[group] - first;
        const auto prefixes = batch.prefixes.subspan(first, rows);
        const auto candidates = keyed.subspan(index * stride, stride);
        const auto paid = std::ranges::any_of(candidates,
            [&](const ec_xonly& key) NOEXCEPT
            {
                return contains(prefixes, array_cast<uint8_t, size>(key));
            });

        if (paid)
        {
            const auto link = from_little_endian(batch.correlates[first]);
            callback({}, link, batch.points[first]);
        }
    }
}

// protected
// ----------------------------------------------------------------------------

size_t silent::batch::next(const batch& batch, size_t row) NOEXCEPT
{
    const auto& correlate = batch.correlates[row];
    const auto count = batch.correlates.size();
    do { ++row; } while (row < count && batch.correlates[row] == correlate);
    return row;
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

    parsed parsed_keys{};
    if (!parse(parsed_keys, keys))
        return;

    std_vector<size_t> chunks(ceilinged_divide(count, chunk_rows));
    std::iota(chunks.begin(), chunks.end(), zero);

    // A chunk owns the transactions that begin within it.
    // Return from scan with !cancel implies complete.
    std::for_each(policy, chunks.cbegin(), chunks.cend(),
        [&](size_t chunk) NOEXCEPT
        {
            if (cancel)
                return;

            auto row = chunk * chunk_rows;
            const auto end = std::min(row + chunk_rows, count);
            if (is_nonzero(row) &&
                batch.correlates[row] == batch.correlates[sub1(row)])
                row = next(batch, row);

            std_vector<size_t> firsts{}, lasts{};
            for (auto last = row; row < end; row = last)
            {
                last = next(batch, row);
                firsts.push_back(row);
                lasts.push_back(last);
            }

            scan_rows(batch, firsts, lasts, parsed_keys, callback);
        });
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
