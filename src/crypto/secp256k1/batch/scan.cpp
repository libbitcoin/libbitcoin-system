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
#include <bitcoin/system/crypto/secp256k1/batch/scan.hpp>

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

class scan_dispatcher
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
    scan_dispatcher::scalar scan{};
    scan_dispatcher::affine spend{};
    scan_dispatcher::affines labels{};
};

static bool parse(parsed& out, const scan::batch::receiver& keys) NOEXCEPT
{
    if (!scan_dispatcher::from_bytes(out.scan, keys.scan) ||
        !scan_dispatcher::from_bytes(out.spend, keys.spend))
        return false;

    out.labels.resize(keys.labels.size());
    for (size_t index{}; index < keys.labels.size(); ++index)
    {
        auto& label = out.labels[index];
        if (!scan_dispatcher::from_bytes(label, keys.labels[index]))
            return false;
    }

    return true;
}

// True if any row of the transaction carries the key prefix.
static bool paid(const std::span<const scan::batch::row_t>& rows,
    const scan::batch::prefix& key) NOEXCEPT
{
    return std::ranges::any_of(rows, [&](const auto& row) NOEXCEPT
    {
        return row.prefix == key;
    });
}

// The shared secret of each transaction (k = 0) tweaks the spend key, and each
// output key (and labeled key) is matched by prefix to the transaction rows.
static void scan_rows(const scan::batch& batch,
    const std::span<const size_t>& firsts,
    const std::span<const size_t>& lasts, const parsed& keys,
    const scan::batch::handler& callback) NOEXCEPT
{
    const auto count = firsts.size();
    std_vector<ec_compressed> points(count);
    for (size_t group{}; group < count; ++group)
        points[group] = batch.rows[firsts[group]].point;

    data_chunk computed{};
    std_vector<ec_compressed> shared{};
    with_lanes([&]<typename Word>() NOEXCEPT
    {
        scan_dispatcher::multiply<Word>(computed, shared, points, keys.scan);
        return true;
    });

    // The tagged hash of each shared point and k = 0 [bip352].
    constexpr auto k = to_big_endian(0_u32);
    constexpr auto& midstate = tagged_midstate<"BIP0352/SharedSecret">;
    scan_dispatcher::scalars tweaks{};
    std_vector<size_t> groups{};
    tweaks.reserve(count);
    groups.reserve(count);
    for (size_t group{}; group < count; ++group)
    {
        if (is_zero(computed[group]))
            continue;

        const auto data = splice(shared[group], k);
        const auto hash = sha256::hash(midstate, data);

        scan_dispatcher::scalar tweak{};
        if (scan_dispatcher::from_bytes(tweak, hash) &&
            !scan_dispatcher::is_zero_scalar(tweak))
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
        scan_dispatcher::tweak<Word>(tweaked, outputs, tweaks, spend, labels);
        return true;
    });

    constexpr auto size = array_count<scan::batch::prefix>;
    const auto stride = add1(keys.labels.size());
    const std::span<const ec_xonly> keyed{ outputs };
    for (size_t index{}; index < groups.size(); ++index)
    {
        if (is_zero(tweaked[index]))
            continue;

        const auto group = groups[index];
        const auto first = firsts[group];
        const auto rows = batch.rows.subspan(first, lasts[group] - first);
        const auto candidates = keyed.subspan(index * stride, stride);
        const auto matched = std::ranges::any_of(candidates,
            [&](const ec_xonly& key) NOEXCEPT
            {
                return paid(rows, array_cast<uint8_t, size>(key));
            });

        if (matched)
        {
            const auto link = from_little_endian(batch.correlates[first]);
            callback({}, link, rows.front().point);
        }
    }
}

// protected
// ----------------------------------------------------------------------------

size_t scan::batch::next(const batch& batch, size_t row) NOEXCEPT
{
    const auto& correlate = batch.correlates[row];
    const auto count = batch.correlates.size();
    do { ++row; } while (row < count && batch.correlates[row] == correlate);
    return row;
}

static_assert(is_same_type<secp256k1::cuda::prefix, scan::batch::prefix>);

// Transactions go to the device in blocks, and the output key prefixes of
// each block are matched to the transaction rows in parallel. A chunk of rows
// owns the transactions that begin within it, and a transaction ends where
// the next begins.
bool scan::batch::scan_device(size_t& resume, const stopper& cancel,
    const batch& batch, const receiver& keys, const handler& callback,
    bool turbo) NOEXCEPT
{
    const auto policy = poolstl::execution::par_if(turbo);
    const auto count = batch.correlates.size();
    const auto stride = add1(keys.labels.size());

    std_vector<size_t> chunks(ceilinged_divide(count, chunk_rows));
    std::iota(chunks.begin(), chunks.end(), zero);
    std_vector<std_vector<size_t>> owned(chunks.size());
    std::for_each(policy, chunks.cbegin(), chunks.cend(),
        [&](size_t chunk) NOEXCEPT
        {
            auto row = chunk * chunk_rows;
            const auto end = std::min(row + chunk_rows, count);
            if (is_nonzero(row) &&
                batch.correlates[row] == batch.correlates[sub1(row)])
                row = next(batch, row);

            for (; row < end; row = next(batch, row))
                owned[chunk].push_back(row);
        });

    std_vector<size_t> firsts{};
    firsts.reserve(add1(count));
    for (const auto& rows: owned)
        firsts.insert(firsts.end(), rows.cbegin(), rows.cend());

    const auto total = firsts.size();
    firsts.push_back(count);

    std_vector<size_t> groups(std::min(device_groups, total));
    std::iota(groups.begin(), groups.end(), zero);
    std_vector<ec_compressed> points{};
    std::vector<secp256k1::cuda::prefix> keys_out{};
    data_chunk valid{};
    for (size_t base{}; base < total && !cancel; base += device_groups)
    {
        const auto size = std::min(device_groups, total - base);
        const auto block = std::span{ groups }.first(size);
        resume = firsts[base];
        points.resize(size);
        std::for_each(policy, block.begin(), block.end(),
            [&](size_t group) NOEXCEPT
            {
                points[group] = batch.rows[firsts[base + group]].point;
            });

        if (!secp256k1::cuda::scan(keys_out, valid, cancel, points, keys))
            return false;

        if (cancel)
            break;

        const std::span<const prefix> keyed{ keys_out };
        std::for_each(policy, block.begin(), block.end(),
            [&](size_t group) NOEXCEPT
            {
                if (is_zero(valid[group]))
                    return;

                const auto first = firsts[base + group];
                const auto last = firsts[add1(base + group)];
                const auto rows = batch.rows.subspan(first, last - first);
                const auto candidates = keyed.subspan(group * stride, stride);
                const auto matched = std::ranges::any_of(candidates,
                    [&](const prefix& key) NOEXCEPT
                    {
                        return paid(rows, key);
                    });

                if (matched)
                {
                    const auto& correlate = batch.correlates[first];
                    const auto link = from_little_endian(correlate);
                    callback({}, link, rows.front().point);
                }
            });
    }

    return true;
}

// scan
// ----------------------------------------------------------------------------
// The callback provides granular response when the query is very long-running,
// which is consistent with the push notification public interface.

void scan::batch::scan(const stopper& cancel, const batch& batch,
    const receiver& keys, const handler& callback, bool turbo) NOEXCEPT
{
    const auto policy = poolstl::execution::par_if(turbo);

    // The spans are corresponding arrays of equal length.
    const auto count = batch.correlates.size();
    BC_ASSERT(batch.rows.size() == count);

    parsed parsed_keys{};
    if (!parse(parsed_keys, keys))
        return;

    size_t resume{};
    if (count >= device_rows &&
        keys.labels.size() <= secp256k1::cuda::maximum_labels &&
        secp256k1::cuda::available() &&
        scan_device(resume, cancel, batch, keys, callback, turbo))
        return;

    std_vector<size_t> chunks(ceilinged_divide(count - resume, chunk_rows));
    std::iota(chunks.begin(), chunks.end(), zero);

    // A chunk owns the transactions that begin within it.
    // Return from scan with !cancel implies complete.
    std::for_each(policy, chunks.cbegin(), chunks.cend(),
        [&](size_t chunk) NOEXCEPT
        {
            if (cancel)
                return;

            auto row = resume + chunk * chunk_rows;
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
