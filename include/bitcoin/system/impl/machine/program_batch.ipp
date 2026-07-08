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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_BATCH_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_BATCH_IPP

#include <iterator>
#include <optional>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// Signature verify (with batching).
// ----------------------------------------------------------------------------
// If compress or parse fails it's not technically a miss (invalid script).

TEMPLATE
inline bool CLASS::
verify_ecdsa_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature, bool capture) const NOEXCEPT
{
    if (capture_.enabled && capture)
    {
        if (is_ecdsa_batchable())
        {
            // Compress failure is a verify failure.
            ec_compressed key;
            if (!to_compressed(key, point))
                return false;

            // Capture is bypass (single sigop).
            capture_.batched.store(true, relaxed);
            if (capture_.ecdsa(hash, key, signature))
                return true;
        }
        else
        {
            // Not batchable.
            capture_.fire(chain::signatures::miss::ecdsa, one);
            capture_.log(*script_);
        }
    }

    return ecdsa::verify_signature(point, hash, signature);
}

TEMPLATE
inline bool CLASS::
try_batch_multisig_verification(const chunk_xptrs& points,
    const chunk_xptrs& endorsements) const NOEXCEPT
{
    if (capture_.enabled)
    {
        if (is_multisig_batchable())
        {
            // Parse failure is a full op execution.
            // TODO: this could be further short-circuited.
            hash_digest hash; ec_compresseds keys; ec_signatures sigs;
            if (!parse_ecdsa_multisig(hash, keys, sigs, points, endorsements))
                return false;

            // Capture is bypass (single sigop).
            capture_.batched.store(true, relaxed);
            if (capture_.multisig(hash, keys, sigs))
                return true;
        }
        else
        {
            // Not batchable.
            capture_.fire(chain::signatures::miss::multisig, points.size());
            capture_.log(*script_);
        }
    }

    // Full op execution.
    return false;
}

// If the store fails to commit the group, the validator must re-introduce the
// block to the queue, as collection otherwise implies the script is valid. And
// threshold collection here cannot be reset once started.
TEMPLATE
inline bool CLASS::
verify_schnorr_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature) const NOEXCEPT
{
    if (capture_.enabled)
    {
        if (is_threshold_batchable())
        {
            if (threshold_.push_tuple(hash, std::cref(as_xonly(point)),
                std::cref(signature)))
            {
                // With a capture fault block success is disregarded.
                capture_.batched.store(true, relaxed);
                if (!capture_.threshold(threshold_))
                    capture_.faulted = true;
            }

            // Push or push/flush (capture) is bypass.
            return true;
        }
        else if (is_schnorr_batchable())
        {
            // Capture is bypass (single sigop).
            if (capture_.schnorr(hash, as_xonly(point), signature))
                return true;
        }
        else
        {
            // Not batchable.
            capture_.fire(chain::signatures::miss::schnorr, one);
            capture_.log(*script_);
        }
    }

    return schnorr::verify_signature(point, hash, signature);
}

TEMPLATE
inline bool CLASS::
is_threshold_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    // Already batching.
    if (!threshold_.tuples.empty())
        return true;

    size_t min{}, max{};
    const auto op = script_->extract_tapscript_threshold(min, max);
    const auto category = chain::threshold::to_category(op);
    if (category == chain::threshold::category_t::unknown)
        return false;

    // All non-empty elements (sigs) on the stack (plus self) must be evaluated
    // in a captured signature op. Underflow/overflow imply script failure.
    const auto expected = add1(stack_nonempty());

    // Limit batching to 65,535 verified; 58,883 is widely observed.
    if (is_limited<uint16_t>(min) ||
        is_limited<uint16_t>(max) ||
        is_limited<uint16_t>(expected))
        return false;

    threshold_.tuples.reserve(expected);
    threshold_.minimum = narrow_cast<uint16_t>(min);
    threshold_.maximum = narrow_cast<uint16_t>(max);
    threshold_.expected = narrow_cast<uint16_t>(expected);
    threshold_.category = category;
    return true;
}

// Batching helpers.
// ----------------------------------------------------------------------------
// private

TEMPLATE
inline bool CLASS::
parse_ecdsa_multisig(hash_digest& hash, ec_compresseds& keys,
    ec_signatures& sigs, const chunk_xptrs& points,
    const chunk_xptrs& endorsements) const NOEXCEPT
{
    uint8_t sighash_flags;
    sigs.reserve(endorsements.size());
    if (!parse_ecdsa_signatures(sighash_flags, sigs, endorsements,
        is_enabled(flags::bip66_rule)))
        return false;

    keys.reserve(points.size());
    if (!compress_public_keys(keys, points))
        return false;

    return signature_hash(hash, *subscript(endorsements), sighash_flags);
}

TEMPLATE
inline bool CLASS::
parse_ecdsa_signatures(uint8_t& sighash, ec_signatures& out,
    const chunk_xptrs& endorsements, bool strict) const NOEXCEPT
{
    std::optional<uint8_t> byte{};
    out.resize(endorsements.size());
    auto it = out.begin();

    for (const auto& sig: endorsements)
    {
        // don't capture ecdsa multisig with empty sigs.
        if (sig->empty())
            return false;

        // enforce uniform byte.
        if (!byte.has_value())
            byte = sig->back();
        else if (byte != sig->back())
            return false;

        const auto end = std::prev(sig->end());
        if (!decode_signature(*it++, { sig->begin(), end }, strict))
            return false;
    }

    // endorsements was empty.
    if (!byte.has_value())
        return false;

    sighash = byte.value();
    return true;
}

TEMPLATE
inline bool CLASS::
compress_public_keys(ec_compresseds& out,
    const chunk_xptrs& keys) const NOEXCEPT
{
    out.resize(keys.size());
    auto it = out.begin();

    for (const auto& key: keys)
        if (!to_compressed(*it++, *key))
            return false;

    return true;
}

TEMPLATE
inline bool CLASS::
to_compressed(ec_compressed& out, const data_chunk& point) const NOEXCEPT
{
    constexpr auto lo = ec_compressed_size;
    constexpr auto hi = ec_uncompressed_size;

    switch (point.size())
    {
        case lo:
            out = unsafe_array_cast<uint8_t, lo>(point.data());
            return true;
        case hi:
            return compress(out, unsafe_array_cast<uint8_t, hi>(point.data()));
        default:
            return false;
    }
}

TEMPLATE
const ec_xonly& CLASS::
as_xonly(const data_chunk& point) const NOEXCEPT
{
    // Guarded by consensus rule.
    BC_ASSERT(point.size() == ec_xonly_size);
    return unsafe_array_cast<uint8_t, ec_xonly_size>(point.data());
}

// Batching properties.
// ----------------------------------------------------------------------------

TEMPLATE
inline bool CLASS::
is_ecdsa_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    const auto& ops = script_->ops();
    return chain::script::is_pay_public_key_pattern(ops)
        || chain::script::is_pay_key_hash_pattern(ops);
}

TEMPLATE
inline bool CLASS::
is_multisig_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    const auto& ops = script_->ops();
    return chain::script::is_pay_multisig_standard_pattern(ops);
}

TEMPLATE
inline bool CLASS::
is_schnorr_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    const auto& ops = script_->ops();
    return chain::script::is_pay_taproot_key_path_pattern(ops)
        || chain::script::is_pay_tapscript_single_pattern(ops)
        || chain::script::is_pay_tapscript_timelock_pattern(ops)
        || chain::script::is_pay_tapscript_inscription_pattern(ops);
}

TEMPLATE
inline bool CLASS::
is_input_script() const NOEXCEPT
{
    return spender_;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
