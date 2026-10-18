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

#include <future>
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
template <size_t Rows>
static void cpu_compute(const stopper& cancel,
    const std::span<ec_compressed>& out,
    const std::span<uint8_t>& valid,
    const std::span<const ec_compressed>& sums,
    const std::span<const ec_secret>& hashes) NOEXCEPT
{
    constexpr auto policy = execution::par;
    using dispatcher = silent_dispatcher;
    const auto count = sums.size();

    std_vector<size_t> chunks(ceilinged_divide(count, Rows));
    std::iota(chunks.begin(), chunks.end(), zero);
    const auto compute = [&](size_t chunk) NOEXCEPT
    {
        if (cancel)
            return;

        const auto first = chunk * Rows;
        const auto size = std::min(Rows, count - first);

        data_chunk parsed(size);
        dispatcher::scalars ks(size);
        for (size_t row{}; row < size; ++row)
        {
            auto& k = ks[row];
            const auto ok = dispatcher::from_bytes(k, hashes[first + row]);
            const auto nonzero = !dispatcher::is_zero_scalar(k);
            parsed[row] = to_int<uint8_t>(ok && nonzero);

            if (is_zero(parsed[row]))
                k = { 1 };
        }

        data_chunk computed{};
        std_vector<ec_compressed> products{};
        with_lanes([&]<typename Word>() NOEXCEPT
        {
            const auto part = sums.subspan(first, size);
            dispatcher::multiply<Word>(computed, products, part, ks);
            return true;
        });

        for (size_t row{}; row < size; ++row)
        {
            const auto ok = to_bool(computed[row]) && to_bool(parsed[row]);
            valid[first + row] = to_int<uint8_t>(ok);
            out[first + row] = products[row];
        }
    };

    std::for_each(policy, chunks.cbegin(), chunks.cend(), compute);
}

bool silent::batch::compute(const stopper& cancel,
    const std::span<ec_compressed>& out, const std::span<uint8_t>& valid,
    const batch& batch) NOEXCEPT
{
    bool use_gpu{ true };
    return compute(cancel, use_gpu, out, valid, batch);
}

bool silent::batch::compute(const stopper& cancel, bool& use_gpu,
    const std::span<ec_compressed>& out, const std::span<uint8_t>& valid,
    const batch& batch) NOEXCEPT
{
    BC_ASSERT(batch.hashes.size() == batch.sums.size());
    BC_ASSERT(valid.size() == batch.sums.size());
    BC_ASSERT(out.size() == batch.sums.size());

    namespace cuda = secp256k1::cuda;

    use_gpu = use_gpu
        && cuda::available()
        && cuda::compute(cancel, out, valid, batch.sums, batch.hashes);

    if (!use_gpu)
        cpu_compute<chunk_rows>(cancel, out, valid, batch.sums, batch.hashes);

    return !cancel;
}

// Gpu computes while caller writes, cpu computes before.
bool silent::batch::compute(const stopper& cancel, bool& use_gpu,
    const std::span<ec_compressed>& out, const std::span<uint8_t>& valid,
    const batch& batch, const std::function<void()>& write) NOEXCEPT
{
    BC_ASSERT(batch.hashes.size() == batch.sums.size());
    BC_ASSERT(valid.size() == batch.sums.size());
    BC_ASSERT(out.size() == batch.sums.size());

    namespace cuda = secp256k1::cuda;

    if (!use_gpu || !cuda::available())
    {
        use_gpu = false;
        cpu_compute<chunk_rows>(cancel, out, valid, batch.sums, batch.hashes);
        write();
        return !cancel;
    }

    auto computed = std::async(std::launch::async, [&]() NOEXCEPT
    {
        return cuda::compute(cancel, out, valid, batch.sums, batch.hashes);
    });

    write();
    use_gpu = computed.get();

    if (!use_gpu)
        cpu_compute<chunk_rows>(cancel, out, valid, batch.sums, batch.hashes);

    return !cancel;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
