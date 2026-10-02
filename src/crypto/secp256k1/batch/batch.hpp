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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_BATCH_HPP

#include <atomic>
#include <numeric>
#include <thread>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/crypto/secp256k1/batch/batch.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/execution.hpp>
#include <bitcoin/system/math/math.hpp>
#include "../cuda/driver.hpp"

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Lanes of Word are available where compiled and the processor executes them.
template <typename Word>
inline bool have_lanes() NOEXCEPT
{
    if constexpr (!have_ifma<Word>)
    {
        return false;
    }
    else if constexpr (is_same_type<Word, xint512_t>)
    {
        static const auto available = try_avx512ifma();
        return available;
    }
    else
    {
        static const auto available = try_avx512ifma() || try_avxifma();
        return available;
    }
}

// Rows verify in the widest available lanes, or integrally.
template <typename Verify>
inline bool with_lanes(Verify&& verify) NOEXCEPT
{
    if constexpr (have_ifma<xint512_t>)
        if (have_lanes<xint512_t>())
            return verify.template operator()<xint512_t>();

    if constexpr (have_ifma<xint256_t>)
        if (have_lanes<xint256_t>())
            return verify.template operator()<xint256_t>();

    return verify.template operator()<uint64_t>();
}

// Chunks of rows verify in parallel, each into its own results, sized to
// occupy the processors twice over but not below a few lane groups.
template <typename Batch, typename Verify>
data_chunk batch_verify_(const stopper& cancel, const Batch& batch,
    Verify&& verify) NOEXCEPT
{
    constexpr auto policy = poolstl::execution::par;
    constexpr size_t minimum = 32;

    const auto count = batch.correlates.size();
    const auto threads = std::max(std::thread::hardware_concurrency(), 1u);
    const auto rows = std::max(minimum, ceilinged_divide(count, two * threads));
    std::vector<size_t> it(ceilinged_divide(count, rows));
    std::iota(it.begin(), it.end(), zero);
    stopper failed{};

    data_chunk results(count);
    std::for_each(policy, it.cbegin(), it.cend(), [&](size_t chunk) NOEXCEPT
    {
        if (cancel) return;
        const auto first = chunk * rows;
        const auto size = std::min(rows, count - first);
        data_chunk out{};
        if (!verify(out, batch.points.subspan(first, size),
            batch.digests.subspan(first, size),
            batch.signatures.subspan(first, size)))
            failed.store(true);

        std::copy(out.begin(), out.end(),
            std::next(results.begin(), to_signed(first)));
    });

    // Empty implies fully-verified batch (or canceled, which caller gates).
    if (cancel || !failed)
        results.clear();

    return results;
}

// Rows verify on the device where available, otherwise or upon its failure
// on the processor.
template <typename Batch, typename Verify>
data_chunk batch_verify(const stopper& cancel, const Batch& batch,
    Verify&& verify) NOEXCEPT
{
    data_chunk results{};
    if (secp256k1::cuda::available() &&
        secp256k1::cuda::verify(results, cancel, batch))
        return results;

    return batch_verify_(cancel, batch, verify);
}

inline void push_fail(batched::links_t& fails,
    const batched::link& id) NOEXCEPT
{
    // Terminal indicates a partially-written signature table row (no-count).
    if (id != batched::terminal)
        fails.push_back(from_little_array<batched::link_t>(id));
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
