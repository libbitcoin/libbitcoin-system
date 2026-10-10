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
#include "batch.hpp"

namespace libbitcoin {
namespace system {

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

class silent_dispatcher
  : public secp256k1::algorithm
{
public:
    using scalars = std_vector<scalar_t>;
    using algorithm::from_bytes;
    using algorithm::is_zero_scalar;
    using algorithm::multiply;
};

// compute
// ----------------------------------------------------------------------------

// Each product = hash * sum, in chunks of lanes on the processor.
static void compute_rows(std_vector<ec_compressed>& products,
    data_chunk& computed, const stopper& cancel,
    const std::span<const ec_compressed>& sums,
    const std::span<const ec_secret>& hashes, size_t chunk_rows) NOEXCEPT
{
    constexpr auto policy = poolstl::execution::par;
    const auto count = sums.size();
    products.assign(count, ec_compressed{});
    computed.assign(count, uint8_t{});

    std_vector<size_t> chunks(ceilinged_divide(count, chunk_rows));
    std::iota(chunks.begin(), chunks.end(), zero);
    std::for_each(policy, chunks.cbegin(), chunks.cend(),
        [&](size_t chunk) NOEXCEPT
        {
            if (cancel)
                return;

            const auto first = chunk * chunk_rows;
            const auto size = std::min(chunk_rows, count - first);
            data_chunk parsed(size);
            silent_dispatcher::scalars ks(size);
            for (size_t row{}; row < size; ++row)
            {
                auto& k = ks[row];
                parsed[row] = to_int<uint8_t>(
                    silent_dispatcher::from_bytes(k, hashes[first + row]) &&
                    !silent_dispatcher::is_zero_scalar(k));

                if (is_zero(parsed[row]))
                    k = { 1 };
            }

            data_chunk valid{};
            std_vector<ec_compressed> out{};
            with_lanes([&]<typename Word>() NOEXCEPT
            {
                silent_dispatcher::multiply<Word>(valid, out,
                    sums.subspan(first, size), ks);
                return true;
            });

            for (size_t row{}; row < size; ++row)
            {
                products[first + row] = out[row];
                computed[first + row] = to_int<uint8_t>(
                    is_nonzero(valid[row]) && is_nonzero(parsed[row]));
            }
        });
}

bool silent::batch::compute(std_vector<ec_compressed>& out,
    data_chunk& valid, const stopper& cancel, const batch& batch) NOEXCEPT
{
    const auto& rows = batch.rows;
    const auto count = rows.size();
    out.assign(count, ec_compressed{});
    valid.assign(count, uint8_t{});

    // The first row of each run of rows of equal sum and hash.
    std_vector<size_t> firsts{};
    for (size_t row{}; row < count; ++row)
        if (is_zero(row) || rows[row].sum != rows[sub1(row)].sum ||
            rows[row].hash != rows[sub1(row)].hash)
            firsts.push_back(row);

    const auto runs = firsts.size();
    std_vector<ec_compressed> sums(runs);
    std_vector<ec_secret> hashes(runs);
    for (size_t run{}; run < runs; ++run)
    {
        sums[run] = rows[firsts[run]].sum;
        hashes[run] = rows[firsts[run]].hash;
    }

    std_vector<ec_compressed> products{};
    data_chunk computed{};
    if (runs < device_rows || !secp256k1::cuda::available() ||
        !secp256k1::cuda::compute(products, computed, cancel, sums, hashes))
        compute_rows(products, computed, cancel, sums, hashes, chunk_rows);

    if (cancel)
        return false;

    firsts.push_back(count);
    for (size_t run{}; run < runs; ++run)
    {
        for (auto row = firsts[run]; row < firsts[add1(run)]; ++row)
        {
            out[row] = products[run];
            valid[row] = computed[run];
        }
    }

    return true;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
