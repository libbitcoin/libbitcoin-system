/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_SIGN_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_SIGN_IPP

#include <iterator>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// Endorsement parsing.
// ----------------------------------------------------------------------------

// static/private
// BIP341: Using any undefined hash_type causes validation failure if violated.
// defined types: 0x00, 0x01, 0x02, 0x03, 0x81, 0x82, or 0x83. [zero is the
// default and cannot be explicit, but is serialized for signature hashing].
TEMPLATE
inline bool CLASS::
is_schnorr_sighash(uint8_t sighash_flags) NOEXCEPT
{
    using namespace chain;

    switch (sighash_flags)
    {
        // BIP341: zero is invalid sighash, must be explicit to prevent mally.
        ////case coverage::hash_default:
        case coverage::hash_all:
        case coverage::hash_none:
        case coverage::hash_single:
        case coverage::all_anyone_can_pay:
        case coverage::none_anyone_can_pay:
        case coverage::single_anyone_can_pay:
            return true;
        default:
            return false;
    }
}

// static
TEMPLATE
inline const ec_signature& CLASS::
schnorr_split(uint8_t& sighash_flags, const data_chunk& endorsement) NOEXCEPT
{
    using namespace chain;
    using namespace schnorr;

    sighash_flags = coverage::invalid;
    const auto size = endorsement.size();

    if (size == signature_size)
    {
        // BIP341: if [sighash byte] is omitted the resulting signatures are 64
        // bytes, and [default == 0] mode is implied (implies SIGHASH_ALL).
        sighash_flags = coverage::hash_default;
    }
    else if (size == add1(signature_size))
    {
        // BIP341: signature has sighash byte appended in the usual fashion.
        const auto byte = endorsement.back();

        // BIP341: Defined sighash type required (otherwise invalid is set).
        if (is_schnorr_sighash(byte))
            sighash_flags = byte;
    }
    else
    {
        // This makes an invalid return safe to dereference, and may be
        // compiled out unless a caller does in fact access it.
        static constexpr ec_signature empty{};
        return empty;
    }

    return unsafe_array_cast<uint8_t, signature_size>(endorsement.data());
}

// static
TEMPLATE
INLINE data_slice CLASS::
ecdsa_split(uint8_t& sighash_flags, const data_chunk& endorsement) NOEXCEPT
{
    BC_ASSERT(!endorsement.empty());
    sighash_flags = endorsement.back();

    // data_slice is returned since the size of the DER encoding is not fixed.
    return { endorsement.begin(), std::prev(endorsement.end()) };
}

// Signature subscripting.
// ----------------------------------------------------------------------------

// Subscripts are referenced by script.offset mutable metadata. This allows for
// efficient subscripting with no copying. As a result concurrent execution of
// any one input script instance is not thread safe.
TEMPLATE
INLINE void CLASS::
set_subscript(const op_iterator& op) NOEXCEPT
{
    // Not possible unless op is not an element of script_.
    BC_ASSERT(!script_->ops().empty() && op != script_->ops().end());

    // Advance the offset to the op following the found code separator.
    // This is non-const because changes script state (despite being mutable).
    script_->offset = std::next(op);
}

// static/private
TEMPLATE
inline chain::strippers CLASS::
create_strip_ops(const chunk_xptrs& endorsements) NOEXCEPT
{
    chain::strippers strip{};
    strip.reserve(add1(endorsements.size()));
    for (const auto& endorsement: endorsements)
        strip.emplace_back(endorsement);

    strip.emplace_back(chain::opcode::codeseparator);
    return strip;
}

// static/private
TEMPLATE
INLINE chain::strippers CLASS::
create_strip_ops(const chunk_xptr& endorsement) NOEXCEPT
{
    using namespace chain;
    return { stripper{ endorsement }, stripper{ opcode::codeseparator } };
}

// ****************************************************************************
// CONSENSUS: Endorsement and code separator stripping are always performed in
// conjunction and are limited to non-witness signature hash subscripts.
// The order of operations is inconsequential, as they are all removed.
// Subscripts are not evaluated, they are limited to signature hash creation.
// ****************************************************************************
TEMPLATE
inline chain::script::cptr CLASS::
subscript(const chunk_xptrs& endorsements) const NOEXCEPT
{
    // bip141: establishes the version property.
    // bip143: op stripping is not applied to bip141 v0 scripts.
    if (is_enabled(flags::bip143_rule) && version_ == script_version::segwit)
        return script_;

    // Transform into a set of endorsement push ops and one op_codeseparator.
    const auto strip = create_strip_ops(endorsements);
    const auto stop = script_->ops().end();
    const op_iterator start{ script_->offset };

    // If none of the strip ops are found, return the subscript.
    if (!is_intersecting<operations>(start, stop, strip))
        return script_;

    // Create new script from stripped copy of subscript operations.
    return to_shared<script>(difference<operations>(start, stop, strip));
}

TEMPLATE
inline chain::script::cptr CLASS::
subscript(const chunk_xptr& endorsement) const NOEXCEPT
{
    // bip141: establishes the version property.
    // bip143: op stripping is not applied to bip141 v0 scripts.
    if (is_enabled(flags::bip143_rule) && version_ == script_version::segwit)
        return script_;

    // Transform into a set with one endorsement push op and op_codeseparator.
    const auto strip = create_strip_ops(endorsement);
    const auto stop = script_->ops().end();
    const op_iterator start{ script_->offset };

    // If none of the strip ops are found, return the subscript.
    if (!is_intersecting<operations>(start, stop, strip))
        return script_;

    // Create new script from stripped copy of subscript operations.
    return to_shared<script>(difference<operations>(start, stop, strip));
}

// Signature hashing.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE bool CLASS::
signature_hash(hash_digest& out, uint8_t sighash_flags) const NOEXCEPT
{
    return signature_hash(out, *script_, sighash_flags);
}

TEMPLATE
INLINE bool CLASS::
signature_hash(hash_digest& out, const script& subscript,
    uint8_t sighash_flags) const NOEXCEPT
{
    return transaction_.signature_hash(out, input_, subscript, value_,
        tapleaf_, version_, sighash_flags, flags_);
}

// Multisig signature hash caching.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE void CLASS::
initialize_cache() NOEXCEPT
{
    cache_.first = true;
}

TEMPLATE
INLINE bool CLASS::
uncached(uint8_t sighash_flags) const NOEXCEPT
{
    return cache_.first || cache_.flags != sighash_flags;
}

TEMPLATE
INLINE bool CLASS::
set_hash(const chain::script& subscript, uint8_t sighash_flags) NOEXCEPT
{
    cache_.first = false;
    cache_.flags = sighash_flags;
    return signature_hash(cache_.hash, subscript, sighash_flags);
}

TEMPLATE
INLINE const hash_digest& CLASS::
cached_hash() const NOEXCEPT
{
    return cache_.hash;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
