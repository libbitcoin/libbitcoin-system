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

// Each out = hash * sum, in chunks of lanes on the processor.
static void compute_rows(const std::span<ec_compressed>& out,
    const std::span<uint8_t>& valid, const stopper& cancel,
    const std::span<const ec_compressed>& sums,
    const std::span<const ec_secret>& hashes, size_t chunk_rows) NOEXCEPT
{
    constexpr auto policy = poolstl::execution::par;
    const auto count = sums.size();

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

            data_chunk computed{};
            std_vector<ec_compressed> products{};
            with_lanes([&]<typename Word>() NOEXCEPT
            {
                silent_dispatcher::multiply<Word>(computed, products,
                    sums.subspan(first, size), ks);
                return true;
            });

            for (size_t row{}; row < size; ++row)
            {
                out[first + row] = products[row];
                valid[first + row] = to_int<uint8_t>(
                    is_nonzero(computed[row]) && is_nonzero(parsed[row]));
            }
        });
}

bool silent::batch::compute(const std::span<ec_compressed>& out,
    const std::span<uint8_t>& valid, const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    bool device{};
    return compute(out, valid, device, cancel, batch);
}

bool silent::batch::compute(const std::span<ec_compressed>& out,
    const std::span<uint8_t>& valid, bool& device, const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    BC_ASSERT(batch.hashes.size() == batch.sums.size());
    BC_ASSERT(out.size() == batch.sums.size());
    BC_ASSERT(valid.size() == batch.sums.size());

    device = secp256k1::cuda::available() &&
        secp256k1::cuda::compute(out, valid, cancel, batch.sums, batch.hashes);

    if (!device)
        compute_rows(out, valid, cancel, batch.sums, batch.hashes, chunk_rows);

    return !cancel;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
