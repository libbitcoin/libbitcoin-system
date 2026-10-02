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
#include <bitcoin/system/crypto/secp256k1/batch/ecdsa.hpp>

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include "batch.hpp"

namespace libbitcoin {
namespace system {

using namespace batched;

// Array indexing required for c++20 span<T>.
BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_VIEW_REFERENCING)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// verify_rows
// ----------------------------------------------------------------------------

class ecdsa_dispatcher
  : public secp256k1::algorithm
{
public:
    using algorithm::verify_ecdsa;

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
};

inline bool verify_rows(data_chunk& results,
    std::span<const ec_compressed> keys, std::span<const hash_digest> digests,
    std::span<const ec_signature> parsed) NOEXCEPT
{
    ec_signatures signatures(parsed.size());
    std::transform(parsed.begin(), parsed.end(), signatures.begin(),
        &ecdsa_dispatcher::canonical);

    return with_lanes([&]<typename Word>() NOEXCEPT
    {
        return ecdsa_dispatcher::verify_ecdsa<Word>(results, keys, digests,
            signatures);
    });
}

// get_failures
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

// evaluate
// ----------------------------------------------------------------------------
// static/protected

data_chunk ecdsa::batch::evaluate(const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    return batch_verify(cancel, batch, verify_rows);
}

// correlate
// ----------------------------------------------------------------------------
// static/protected

links_t ecdsa::batch::correlate(const stopper& cancel,
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

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
