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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_CONSTRUCT_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_PROGRAM_CONSTRUCT_IPP

#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// Constructors.
// ----------------------------------------------------------------------------

// Input script run (default/empty stack).
// 'tx' must remain in scope, this holds state referenced by weak pointers.
// This expectation is guaranteed by the retained tx reference.
TEMPLATE
inline CLASS::
program(const transaction& tx, const input_iterator& input,
    uint32_t active_flags) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_((*input)->script_ptr()),
    flags_(bit_and(active_flags, bip342_mask)),
    value_(max_uint64),
    version_(script_version::unversioned),
    primary_()
{
    script_->clear_offset();
}

// Legacy p2sh or prevout script run (copied input stack - use first).
// 'other' must remain in scope, this holds state referenced by weak pointers.
// This expectation is guaranteed by the retained transaction_ member reference
// and copied program tether (which is not tx state).
TEMPLATE
inline CLASS::
program(const program& other, const script::cptr& script) NOEXCEPT
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    flags_(other.flags_),
    value_(other.value_),
    version_(other.version_),
    primary_(other.primary_)
{
    script_->clear_offset();
}

// Legacy p2sh or prevout script run (moved input stack/tether - use last).
TEMPLATE
inline CLASS::
program(program&& other, const script::cptr& script) NOEXCEPT
  : transaction_(other.transaction_),
    input_(other.input_),
    script_(script),
    flags_(other.flags_),
    value_(other.value_),
    version_(other.version_),
    primary_(std::move(other.primary_))
{
    script_->clear_offset();
}

// Segwit script run (witness-initialized stack).
// 'tx', 'input' (and iterated chain::input) must remain in scope, as these
// hold chunk state weak references. A witness pointer is explicitly retained
// to guarantee the lifetime of its elements.
TEMPLATE
inline CLASS::
program(const transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t active_flags,
    script_version version, const chunk_cptrs_ptr& witness) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_(script),
    flags_(bit_and(active_flags, bip342_mask)),
    value_((*input)->prevout->value()),
    version_(version),
    witness_(witness),
    primary_(projection<Stack>(*witness))
{
    script_->clear_offset();
}

// Taproot script run (witness-initialized stack).
// Same as segwit but with tapleaf, budget, and unstripped bip342 flag.
// Sigop budget is 50 plus size of prefixed serialized witness [bip342].
// Budget is initialized add1(50) to make it zero-based, avoiding signed type.
// This program is never used to construct another, so masked flags_ never mix.
TEMPLATE
inline CLASS::
program(const transaction& tx, const input_iterator& input,
    const script::cptr& script, uint32_t active_flags,
    script_version version, const chunk_cptrs_ptr& witness,
    const hash_cptr& tapleaf) NOEXCEPT
  : transaction_(tx),
    input_(input),
    script_(script),
    flags_(active_flags),
    value_((*input)->prevout->value()),
    version_(version),
    witness_(witness),
    tapleaf_(tapleaf),
    primary_(projection<Stack>(*witness)),
    budget_(ceilinged_add(
        add1(chain::signature_cost),
        chain::witness::serialized_size(*witness_, true)))
{
    script_->clear_offset();
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
