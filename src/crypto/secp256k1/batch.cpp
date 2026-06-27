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
#include <bitcoin/system/crypto/secp256k1_batch.hpp>

#include <atomic>
#include <algorithm>
#include <numeric>
#include <shared_mutex>
#include <span>
#if defined(HAVE_ULTRAFAST)
    #include <ufsecp_libbitcoin.h>
#endif
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/execution.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

using namespace chain;
using namespace batched;

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// polymorphic namespace selectors (template support)
// ----------------------------------------------------------------------------
// local

inline bool verify_signature(const ecdsa::batch& single) NOEXCEPT
{
    return ecdsa::verify_signature(single.point, single.digest,
        single.signature);
}

inline bool verify_signature(const schnorr::batch& single) NOEXCEPT
{
    return schnorr::verify_signature(single.point, single.digest,
        single.signature);
}

// batch_verify
// ----------------------------------------------------------------------------

#if defined(HAVE_ULTRAFAST)

template <typename Batch>
data_chunk batch_verify(const stopper& cancel,
    const std::span<Batch>& batch) NOEXCEPT
{
    // OOM is unrecoverable.
    static thread_local ufsecp::lbtc::Controller context{};
    if (!context.ok()) std::abort();

    // set up cancellation callback.
    const ufsecp_cancel_fn callback = [](const void* atomic) NOEXCEPT
    {
        constexpr auto relaxed = std::memory_order_relaxed;
        return to_int(pointer_cast<const stopper>(atomic)->load(relaxed));
    };

    const auto count = batch.size();
    data_chunk results(count);
    const auto out = results.data();
    const auto in = pointer_cast<const uint8_t>(batch.data());
    const ufsecp_cancel_token token{ callback, &cancel, 0 };

    if constexpr (is_same_type<Batch, schnorr::batch>)
    {
        constexpr auto extra_size = sizeof(Batch) - (sizeof(hash_digest) +
            sizeof(ec_xonly) + sizeof(ec_signature));
        ufsecp_lbtc_verify_schnorr_mt(context.get(), in, count, extra_size,
            out, nullptr, 0, nullptr, 0, &token);
    }
    else
    {
        constexpr auto extra_size = sizeof(Batch) - (sizeof(hash_digest) +
            sizeof(ec_compressed) + sizeof(ec_signature));
        ufsecp_lbtc_verify_ecdsa_opaque_mt(context.get(), in, count, extra_size,
            out, nullptr, 0, nullptr, 0, &token);
    }

    return results;
}

#else

template <typename Batch>
data_chunk batch_verify(const stopper& cancel,
    const std::span<Batch>& batch) NOEXCEPT
{
    constexpr auto policy = poolstl::execution::par;

    // Used only to produce order for concurrency.
    std::vector<size_t> it(batch.size());
    std::iota(it.begin(), it.end(), zero);

    // Collect signature validation results as corresponding integer booleans.
    data_chunk results(batch.size());
    std::for_each(policy, it.cbegin(), it.cend(), [&](size_t row) NOEXCEPT
    {
        if (cancel) return;
        results.at(row) = to_int<uint8_t>(verify_signature(batch[row]));
    });

    return results;
}

#endif

// local
// ----------------------------------------------------------------------------
inline void push_fail(links_t& fails, const batched::link& id) NOEXCEPT
{
    fails.push_back(from_little_array<link_t>(id));
}

// get_failures (ecdsa)
// ----------------------------------------------------------------------------

// O(1) as m and n are bounded at 16.
bool ecdsa::batch::meets_threshold(uint8_t signatures, uint8_t keys,
    const multisig_matrix& successes) NOEXCEPT
{
    BC_ASSERT(!is_limited(signatures, 1, 16));
    BC_ASSERT(!is_limited(keys, 1, 16));
    if (signatures > keys)
        return false;

    uint8_t key{};
    for (uint8_t signature{}; signature < signatures; ++signature)
    {
        bool matched{};
        while (key < keys && !matched)
            if (get_right(successes.at(signature), key++))
                matched = true;

        if (!matched)
            return false;
    }

    return true;
}

// Ecdsa (single sig and standard multisig) correlation.
// O(n) over the sig set, ~100 bytes of stack, no heap.
links_t ecdsa::batch::get_failures(const stopper& cancel,
    const data_chunk& out, const span& in) NOEXCEPT
{
    BC_ASSERT(out.size() == in.size());

    size_t group{};
    links_t fails{};
    for (auto index = one; index <= in.size() && !cancel; ++index)
    {
        // Find the start of the next group (or end).
        if ((index != in.size()) &&
            (in[index].id == in[group].id) &&
            (in[index].group == in[group].group))
            continue;

        // Short-circuit single signature.
        const auto second = add1(group);
        const auto single = (index == second);
        if (single)
        {
            if (!to_bool(out.at(group)))
                push_fail(fails, in[group].id);

            group = index;
            continue;
        }

        // Build matrix and determine effective m/n from (sig, key) pairs.
        multisig_matrix successes{};
        uint8_t max_sig{}, max_key{};

        for (auto row = group; row < index; ++row)
        {
            const auto [sig, key] = unpack_word<uint8_t>(in[row].pair);
            if (to_bool(out.at(row)))
                set_right_into(successes.at(sig), key);

            max_sig = greater(sig, max_sig);
            max_key = greater(key, max_key);
        }

        // Evaluate op_checkmultisig success.
        if (!meets_threshold(add1(max_sig), add1(max_key), successes))
            push_fail(fails, in[group].id);

        group = index;
    }

    return fails;
}

// get_failures (schnorr)
// ----------------------------------------------------------------------------

bool schnorr::batch::meets_threshold(uint8_t category, size_t successes,
    size_t minimum, size_t maximum) NOEXCEPT
{
    switch (static_cast<threshold::category_t>(category))
    {
        case threshold::category_t::equal:
            return (successes == minimum);
        case threshold::category_t::inequal:
            return (successes != minimum);
        case threshold::category_t::lesser:
            return is_lesser(successes, minimum);
        case threshold::category_t::greater:
            return is_greater(successes, minimum);
        case threshold::category_t::not_greater:
            return !is_greater(successes, minimum);
        case threshold::category_t::not_lesser:
            return !is_lesser(successes, minimum);
        case threshold::category_t::between:
            return !is_limited(successes, minimum, maximum);
        default:
            return false;
    }
}

// Schnorr (single sig and threshold sigs) correlation.
// O(n) over the sig set, ~100 bytes of stack, no heap.
links_t schnorr::batch::get_failures(const stopper& cancel,
    const data_chunk& out, const span& in) NOEXCEPT
{
    BC_ASSERT(out.size() == in.size());

    size_t group{};
    links_t fails{};
    for (auto index = one; index <= in.size() && !cancel; ++index)
    {
        // Find the start of the next group (or end).
        if ((index != in.size()) &&
            (in[index].id == in[group].id) &&
            (in[index].group == in[group].group))
            continue;

        // Short-circuit single signature.
        const auto first = group;
        const auto second = add1(group);
        const auto single = (index == second);
        if (single)
        {
            if (!to_bool(out.at(group)))
                push_fail(fails, in[group].id);

            group = index;
            continue;
        }

        // Count successes in the group.
        size_t successes{};
        for (auto row = group; row < index; ++row)
            if (to_bool(out.at(row)))
                ++successes;

        // Get min and max from first two rows (max only for op_within).
        const auto minimum = in[first].pair;
        const auto maximum = in[second].pair;
        const auto category = in[first].category;
        if (!meets_threshold(category, successes, minimum, maximum))
            push_fail(fails, in[group].id);

        group = index;
    }

    return fails;
}

// get_match (silent)
// ----------------------------------------------------------------------------

bool silent::batch::get_match(tx_link_t& , const batch& , size_t ,
    const ec_secret& ) NOEXCEPT
{
    // TODO: implement.
    return false;
}

// evaluate
// ----------------------------------------------------------------------------
// static/protected

data_chunk ecdsa::batch::evaluate(const stopper& cancel,
    const span& batch) NOEXCEPT
{
    return batch_verify(cancel, batch);
}

data_chunk schnorr::batch::evaluate(const stopper& cancel,
    const span& batch) NOEXCEPT
{
    return batch_verify(cancel, batch);
}

// correlate
// ----------------------------------------------------------------------------
// static/protected

links_t ecdsa::batch::correlate(const stopper& cancel,
    const data_chunk& out, const span& in) NOEXCEPT
{
    return get_failures(cancel, out, in);
}

links_t schnorr::batch::correlate(const stopper& cancel,
    const data_chunk& out, const span& in) NOEXCEPT
{
    return get_failures(cancel, out, in);
}

// verify
// ----------------------------------------------------------------------------
// static/public

links_t ecdsa::batch::verify(const stopper& cancel,
    const span& batch) NOEXCEPT
{
    return correlate(cancel, evaluate(cancel, batch), batch);
}

links_t schnorr::batch::verify(const stopper& cancel,
    const span& batch) NOEXCEPT
{
    return correlate(cancel, evaluate(cancel, batch), batch);
}

// scan
// ----------------------------------------------------------------------------
// The callback provides granular response when the query is very long-running,
// which is consistent with the push notification public interface.

#if defined(HAVE_ULTRAFAST)

void silent::batch::scan(const stopper& , const batch& ,
    const ec_secret& , const handler& ) NOEXCEPT
{
    // TODO: iterate over chunked subsets or entire set.
    // TODO: correlate matches to tx link column and invoke callback.
}

#else

void silent::batch::scan(const stopper& cancel, const batch& batch,
    const ec_secret& scan_key, const handler& callback, bool turbo) NOEXCEPT
{
    const auto policy = poolstl::execution::par_if(turbo);

    // Three spans are corresponding arrays of equal length.
    const auto count = batch.correlates.size();
    BC_ASSERT(batch.prefixes.size() == count);
    BC_ASSERT(batch.compresseds.size() == count);

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

#endif

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
