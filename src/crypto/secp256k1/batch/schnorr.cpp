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
#include <bitcoin/system/crypto/secp256k1/batch/schnorr.hpp>

#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
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

class schnorr_dispatcher
  : public secp256k1::algorithm
{
public:
    using algorithm::verify_schnorr;

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

// Integral rows verify by one random linear combination, which identifies no
// failing row, so rows verify alone where it fails.
inline bool verify_rows(data_chunk& results,
    const std::span<const schnorr::batch::row_t>& rows) NOEXCEPT
{
    const auto count = rows.size();
    ec_xonlys keys(count);
    hashes challenges(count);
    ec_signatures signatures(count);
    for (size_t row{}; row < count; ++row)
    {
        keys[row] = rows[row].point;
        signatures[row] = rows[row].signature;
        challenges[row] = schnorr_dispatcher::challenge(signatures[row],
            keys[row], rows[row].digest);
    }

    return with_lanes([&]<typename Word>() NOEXCEPT
    {
        if constexpr (is_same_type<Word, uint64_t>)
        {
            if (schnorr_dispatcher::verify_schnorr(keys, challenges, signatures))
            {
                results.assign(keys.size(), uint8_t{ 1 });
                return true;
            }
        }

        return schnorr_dispatcher::verify_schnorr<Word>(results, keys, challenges,
            signatures);
    });
}

// get_failures
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

// evaluate
// ----------------------------------------------------------------------------
// static/protected

data_chunk schnorr::batch::evaluate(bool& device, const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    return batch_verify(device, cancel, batch, verify_rows);
}

// correlate
// ----------------------------------------------------------------------------
// static/protected

links_t schnorr::batch::correlate(const stopper& cancel,
    const data_chunk& out, const batch& in) NOEXCEPT
{
    return get_failures(cancel, out, in);
}

// verify
// ----------------------------------------------------------------------------
// static/public

links_t schnorr::batch::verify(const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    bool device{};
    return verify(device, cancel, batch);
}

links_t schnorr::batch::verify(bool& device, const stopper& cancel,
    const batch& batch) NOEXCEPT
{
    return correlate(cancel, evaluate(device, cancel, batch), batch);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
