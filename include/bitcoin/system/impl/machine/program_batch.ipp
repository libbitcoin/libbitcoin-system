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
// Store declines (allocation/commit) set faulted and verify inline. All other
// capture fallbacks under a batchable pattern are counted as missed.

TEMPLATE
inline bool CLASS::
verify_ecdsa_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature, bool capture) const NOEXCEPT
{
    if (capture_.enabled && capture)
    {
        if (is_ecdsa_batchable())
        {
            ec_compressed key;
            if (!to_compressed(key, point))
            {
                // Count as missed, consistent with not batchable telemetry.
                capture_.fire(chain::signatures::miss::ecdsa, one);
                capture_.log(*script_);
                return ecdsa::verify_signature(point, hash, signature);
            }

            // Capture is bypass (single sigop).
            if (capture_.ecdsa(hash, key, signature))
            {
                capture_.batched.store(true, relaxed);
                return true;
            }

            // Store fault, verify inline, recoverable if disk full.
            capture_.faulted.store(true, relaxed);
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
    using namespace chain;
    if (!capture_.enabled)
        return false;

    if (!is_multisig_batchable())
    {
        // Not batchable.
        capture_.fire(signatures::miss::multisig, points.size());
        capture_.log(*script_);
        return false;
    }

    // Stack-only normalization (sigs, keys <= 16 by standard pattern).
    const auto m = endorsements.size();
    const auto n = points.size();
    BC_ASSERT(multisig::check(m, n));

    // Parse failure is a full op execution (mixed sighash or invalid).
    sigs_array sigs; keys_array keys; hash_digest hash;
    if (!parse_ecdsa_multisig(hash, keys, sigs, points, endorsements))
    {
        // Count as missed, consistent with not batchable telemetry.
        capture_.fire(signatures::miss::multisig, n);
        capture_.log(*script_);
        return false;
    }

    // Allocate all banded rows, terminal implies store decline.
    auto sink = capture_.multisig(multisig::rows(m, n));
    if (!sink.is_open())
    {
        // Store fault, verify inline, recoverable if disk full.
        capture_.faulted.store(true, relaxed);
        return false;
    }

    // Stream the band: sig i pairs keys [i, i + (n - m)], closes on last.
    const auto gap = n - m;
    for (size_t sig{}; sig < m; ++sig)
        for (auto key = sig; key <= gap + sig; ++key)
            sink.write(hash, keys[key], sigs[sig],
                pack_word<uint8_t>(sig, key));

    // Capture is bypass (single sigop).
    capture_.batched.store(true, relaxed);
    return true;
}

TEMPLATE
inline bool CLASS::
verify_schnorr_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature) const NOEXCEPT
{
    if (capture_.enabled)
    {
        if (is_threshold_batchable())
        {
            // Capture is bypass (rows preallocated at gate).
            threshold_.sink.write(hash, as_xonly(point), signature);
            return true;
        }
        else if (is_schnorr_batchable())
        {
            // Capture is bypass (single sigop).
            if (capture_.schnorr(hash, as_xonly(point), signature))
            {
                capture_.batched.store(true, relaxed);
                return true;
            }

            // Store fault, verify inline, recoverable if disk full.
            capture_.faulted.store(true, relaxed);
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
    if (threshold_.sink.is_open())
        return true;

    size_t min{}, max{};
    const auto op = script_->extract_tapscript_threshold(min, max);

    // All non-empty elements (sigs) on the stack (plus self) must be
    // evaluated in a captured sigop. Underflow/overflow imply failure.
    const auto expected = add1(stack_nonempty());
    if (!chain::threshold::is_satisfied(op, expected, min, max))
        return false;

    // Allocate all rows.
    threshold_.sink = capture_.threshold(expected);
    if (!threshold_.sink.is_open())
    {
        // Store fault, verify inline, recoverable if disk full.
        capture_.faulted.store(true, relaxed);
        return false;
    }

    capture_.batched.store(true, relaxed);
    return true;
}

// Batching helpers.
// ----------------------------------------------------------------------------
// private

TEMPLATE
inline bool CLASS::
parse_ecdsa_multisig(hash_digest& hash, keys_array& keys,
    sigs_array& sigs, const chunk_xptrs& points,
    const chunk_xptrs& endorsements) const NOEXCEPT
{
    uint8_t sighash;
    const auto bip66 = is_enabled(flags::bip66_rule);
    return 
        parse_ecdsa_signatures(sighash, sigs, endorsements, bip66) &&
        compress_public_keys(keys, points) &&
        signature_hash(hash, *subscript(endorsements), sighash);
}

TEMPLATE
inline bool CLASS::
parse_ecdsa_signatures(uint8_t& sighash, sigs_array& out,
    const chunk_xptrs& endorsements, bool strict) const NOEXCEPT
{
    BC_ASSERT(endorsements.size() <= out.size());
    std::optional<uint8_t> byte{};
    auto it = out.begin();

    for (const auto& sig: endorsements)
    {
        // Don't capture ecdsa multisig with empty sigs.
        if (sig->empty())
            return false;

        // Enforce uniform sighash byte (single digest per group).
        if (!byte.has_value())
            byte = sig->back();
        else if (byte != sig->back())
            return false;

        const auto end = std::prev(sig->end());
        if (!decode_signature(*it++, { sig->begin(), end }, strict))
            return false;
    }

    // Endorsements was empty.
    if (!byte.has_value())
        return false;

    sighash = byte.value();
    return true;
}

TEMPLATE
inline bool CLASS::
compress_public_keys(keys_array& out,
    const chunk_xptrs& keys) const NOEXCEPT
{
    BC_ASSERT(keys.size() <= out.size());
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
