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

TEMPLATE
INLINE bool CLASS::
verify_ecdsa_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature) const NOEXCEPT
{
    if (!capture_.enabled || !is_ecdsa_batchable())
    {
        capture_.unbatched_ecdsa.fetch_add(one, relaxed);
        return ecdsa::verify_signature(point, hash, signature);
    }

    ec_compressed key;
    if (!to_compressed(key, point))
        return false;

    capture_.batched_ecdsa.fetch_add(one, relaxed);
    capture_.ecdsa(hash, key, signature);
    return true;
}

TEMPLATE
INLINE bool CLASS::
verify_schnorr_signature(const data_chunk& point, const hash_digest& hash,
    const ec_signature& signature, bool batchable) const NOEXCEPT
{
    if (!batchable || !capture_.enabled || !is_schnorr_batchable())
    {
        capture_.unbatched_schnorr.fetch_add(one, relaxed);
        return schnorr::verify_signature(point, hash, signature);
    }

    capture_.batched_schnorr.fetch_add(one, relaxed);
    constexpr auto size = schnorr::public_key_size;
    const auto& x_point = unsafe_array_cast<uint8_t, size>(point.data());
    capture_.schnorr(hash, x_point, signature);
    return true;
}

TEMPLATE
INLINE bool CLASS::
try_batch_multisig_verification(const chunk_xptrs& points,
    const chunk_xptrs& endorsements) const NOEXCEPT
{
    if (!capture_.enabled || !is_multisig_batchable())
    {
        capture_.unbatched_multisig.fetch_add(endorsements.size(), relaxed);
        return false;
    }

    ec_compresseds keys(points.size());
    if (!compress_public_keys(keys, points))
        return false;

    uint8_t sighash_flags;
    ec_signatures sigs(endorsements.size());
    if (!parse_ecdsa_signatures(sighash_flags, sigs, endorsements,
        is_enabled(flags::bip66_rule)))
        return false;

    hash_digest hash;
    if (!signature_hash(hash, *subscript(endorsements), sighash_flags))
        return false;

    // The group is storage limited to uint16 but capture group is uint64.
    using storage_group_t = decltype(multisig::batch::group);
    using capture_group_t = decltype(capture_.group)::value_type;
    static_assert(is_same_type<capture_group_t, uint64_t>);
    static_assert(is_same_type<storage_group_t, uint16_t>);

    // So an overflow can be safely detected here, resulting in capture bypass.
    // Caller can detect occurances of lost capture by checking capture_.group.
    const auto group = capture_.group.fetch_add(one, relaxed);
    if (is_limited<storage_group_t>(group))
        return false;

    capture_.batched_multisig.fetch_add(sigs.size(), relaxed);
    capture_.multisig(hash, keys, sigs, narrow_cast<storage_group_t>(group));
    return true;
}

// Batching helpers.
// ----------------------------------------------------------------------------
// private

// static
TEMPLATE
inline bool CLASS::
compress_public_keys(ec_compresseds& out, const chunk_xptrs& keys) NOEXCEPT
{
    out.resize(keys.size());
    auto it = out.begin();

    for (const auto& key: keys)
        if (!to_compressed(*it++, *key))
            return false;

    return true;
}

// static
TEMPLATE
inline bool CLASS::
parse_ecdsa_signatures(uint8_t& sighash, ec_signatures& out,
    const chunk_xptrs& endorsements, bool strict) NOEXCEPT
{
    std::optional<uint8_t> byte{};
    out.resize(endorsements.size());
    auto it = out.begin();

    for (const auto& sig: endorsements)
    {
        if (sig->empty())
            return false;

        // enforce uniform byte.
        if (!byte.has_value())
            byte = sig->back();
        else if (byte != sig->back())
            return false;

        const auto end = std::prev(sig->end());
        if (!ecdsa::parse_signature(*it++, { sig->begin(), end }, strict))
            return false;
    }

    // endorsements was empty.
    if (!byte.has_value())
        return false;

    sighash = byte.value();
    return true;
}

// static
TEMPLATE
bool CLASS::
to_compressed(ec_compressed& out, const data_chunk& point) NOEXCEPT
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
INLINE bool CLASS::
is_multisig_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    const auto& ops = script_->ops();
    return chain::script::is_pay_multisig_standard_pattern(ops);
}

TEMPLATE
INLINE bool CLASS::
is_schnorr_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    const auto& ops = script_->ops();
    return chain::script::is_pay_witness_taproot_key_path_pattern(ops);
}

TEMPLATE
INLINE bool CLASS::
is_ecdsa_batchable() const NOEXCEPT
{
    if (is_input_script())
        return false;

    const auto& ops = script_->ops();
    return chain::script::is_pay_public_key_pattern(ops)
        || chain::script::is_pay_key_hash_pattern(ops)
        || chain::script::is_pay_witness_key_hash_pattern(ops);
}

TEMPLATE
INLINE bool CLASS::
is_input_script() const NOEXCEPT
{
    return spender_;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
