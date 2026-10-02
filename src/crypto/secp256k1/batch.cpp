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
#include <bitcoin/system/crypto/secp256k1_batch.hpp>

#include <atomic>
#include <numeric>
#include <span>
#include <thread>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/execution.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "cuda/driver.hpp"

namespace libbitcoin {
namespace system {

using namespace chain;
using namespace batched;

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// accelerated
// ----------------------------------------------------------------------------

bool batched::compiled() NOEXCEPT
{
    return secp256k1::cuda::compiled();
}

bool batched::accelerated() NOEXCEPT
{
    return secp256k1::cuda::available();
}

// batch_verify
// ----------------------------------------------------------------------------

class dispatcher
  : public secp256k1::algorithm
{
public:
    using algorithm::verify_ecdsa;
    using algorithm::verify_schnorr;

    // A parsed ecdsa signature is r then s as native words.
    static ec_signature canonical(const ec_signature& parsed) NOEXCEPT
    {
        constexpr auto size = array_count<bytes_t>;
        constexpr auto words = array_count<scalar_t>;
        ec_signature out{};
        to_bytes(array_cast<uint8_t, size>(out),
            array_cast<uint64_t, words>(parsed));
        to_bytes(array_cast<uint8_t, size, size>(out),
            array_cast<uint64_t, words, size>(parsed));
        return out;
    }

    static hash_digest challenge(const ec_signature& signature,
        const ec_xonly& key, const hash_digest& digest) NOEXCEPT
    {
        accumulator<sha256> hasher{ tagged_midstate<"BIP0340/challenge">,
            one };
        hasher.write(array_cast<uint8_t, array_count<bytes_t>>(signature));
        hasher.write(key);
        hasher.write(digest);
        return hasher.flush();
    }
};

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

inline bool verify_rows(data_chunk& results,
    std::span<const ec_compressed> keys, std::span<const hash_digest> digests,
    std::span<const ec_signature> parsed) NOEXCEPT
{
    ec_signatures signatures(parsed.size());
    std::transform(parsed.begin(), parsed.end(), signatures.begin(),
        &dispatcher::canonical);

    return with_lanes([&]<typename Word>() NOEXCEPT
    {
        return dispatcher::verify_ecdsa<Word>(results, keys, digests,
            signatures);
    });
}

// Integral rows verify by one random linear combination, which identifies no
// failing row, so rows verify alone where it fails.
inline bool verify_rows(data_chunk& results, std::span<const ec_xonly> keys,
    std::span<const hash_digest> digests,
    std::span<const ec_signature> signatures) NOEXCEPT
{
    hashes challenges(keys.size());
    for (size_t row{}; row < keys.size(); ++row)
        challenges[row] = dispatcher::challenge(signatures[row], keys[row],
            digests[row]);

    return with_lanes([&]<typename Word>() NOEXCEPT
    {
        if constexpr (is_same_type<Word, uint64_t>)
        {
            if (dispatcher::verify_schnorr(keys, challenges, signatures))
            {
                results.assign(keys.size(), uint8_t{ 1 });
                return true;
            }
        }

        return dispatcher::verify_schnorr<Word>(results, keys, challenges,
            signatures);
    });
}

// Chunks of rows verify in parallel, each into its own results, sized to
// occupy the processors twice over but not below a few lane groups.
template <typename Batch>
data_chunk batch_verify_(const stopper& cancel, const Batch& batch) NOEXCEPT
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
        if (!verify_rows(out, batch.points.subspan(first, size),
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
template <typename Batch>
data_chunk batch_verify(const stopper& cancel, const Batch& batch) NOEXCEPT
{
    data_chunk results{};
    if (secp256k1::cuda::available() &&
        secp256k1::cuda::verify(results, cancel, batch))
        return results;

    return batch_verify_(cancel, batch);
}

// local
// ----------------------------------------------------------------------------

inline void push_fail(links_t& fails, const batched::link& id) NOEXCEPT
{
    // Terminal indicates a partially-written signature table row (no-count).
    if (id != batched::terminal)
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
    const data_chunk& out, const batch& in) NOEXCEPT
{
    const auto& correlates = in.correlates;
    BC_ASSERT(out.empty() || out.size() == correlates.size());

    // Empty implies fully-verified batch (or canceled, which the caller
    // gates on the stopper, discarding the links).
    links_t fails{};
    if (out.empty())
        return fails;

    size_t group{};
    for (auto index = one; index <= correlates.size() && !cancel; ++index)
    {
        // Find the start of the next group (or end).
        if ((index != correlates.size()) &&
            (correlates[index].id == correlates[group].id) &&
            (correlates[index].group == correlates[group].group))
            continue;

        // Short-circuit single signature.
        const auto second = add1(group);
        const auto single = (index == second);
        if (single)
        {
            if (!to_bool(out.at(group)))
                push_fail(fails, correlates[group].id);

            group = index;
            continue;
        }

        // Build matrix and determine effective m/n from (sig, key) pairs.
        multisig_matrix successes{};
        uint8_t max_sig{}, max_key{};

        for (auto row = group; row < index; ++row)
        {
            const auto [sig, key] = unpack_word<uint8_t>(correlates[row].pair);
            if (to_bool(out.at(row)))
                set_right_into(successes.at(sig), key);

            max_sig = greater(sig, max_sig);
            max_key = greater(key, max_key);
        }

        // Evaluate op_checkmultisig success.
        if (!meets_threshold(add1(max_sig), add1(max_key), successes))
            push_fail(fails, correlates[group].id);

        group = index;
    }

    return distinct(std::move(fails));
}

// get_failures (schnorr)
// ----------------------------------------------------------------------------

// Schnorr (single sig and threshold sigs) correlation.
// Capture is gated on the comparison outcome at full success (see
// is_threshold_batchable), and bip342 terminates the script on any failing
// non-empty signature, so any failed row fails its block. No group or category
// correlation required (contrast ecdsa::get_failures, where op_checkmultisig
// has combinatorial semantics).
links_t schnorr::batch::get_failures(const stopper& cancel,
    const data_chunk& out, const batch& in) NOEXCEPT
{
    const auto& correlates = in.correlates;
    BC_ASSERT(out.empty() || out.size() == correlates.size());

    // Empty implies fully-verified batch (or canceled, which the caller
    // gates on the stopper, discarding the links).
    links_t fails{};
    if (out.empty())
        return fails;

    for (size_t row{}; row < correlates.size() && !cancel; ++row)
        if (!to_bool(out.at(row)))
            push_fail(fails, correlates[row].id);

    return distinct(std::move(fails));
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
    const batch& batch) NOEXCEPT
{
    return batch_verify(cancel, batch);
}

data_chunk schnorr::batch::evaluate(const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    return batch_verify(cancel, batch);
}

// correlate
// ----------------------------------------------------------------------------
// static/protected

links_t ecdsa::batch::correlate(const stopper& cancel,
    const data_chunk& out, const batch& in) NOEXCEPT
{
    return get_failures(cancel, out, in);
}

links_t schnorr::batch::correlate(const stopper& cancel,
    const data_chunk& out, const batch& in) NOEXCEPT
{
    return get_failures(cancel, out, in);
}

// verify
// ----------------------------------------------------------------------------
// static/public

links_t ecdsa::batch::verify(const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    return correlate(cancel, evaluate(cancel, batch), batch);
}

links_t schnorr::batch::verify(const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    return correlate(cancel, evaluate(cancel, batch), batch);
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
