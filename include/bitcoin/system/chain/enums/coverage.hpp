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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ENUMS_COVERAGE_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ENUMS_COVERAGE_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// Signature hash types.
/// Comments from: bitcoin.org/en/developer-guide#standard-transactions
enum coverage : uint8_t
{
    /// Used to mask unused bits in the signature hash byte.
    mask = unmask_right<uint8_t>(5),

    /// Used to set and select bits here and in signature hashing.
    hash_all_bit = 0,
    hash_none_bit = 1,
    anyone_can_pay_bit = 7,

    /// BIP341: We define a new hashtype SIGHASH_DEFAULT (value 0x00) which
    /// results in signing over the whole transaction just as for SIGHASH_ALL.
    hash_default = 0,

    /// The default, signs all the inputs and outputs, protecting everything
    /// except the signature scripts against modification.
    hash_all = bit_right<uint8_t>(hash_all_bit),

    /// Signs all of the inputs but none of the outputs, allowing anyone to
    /// change where the satoshis are going unless other signatures using
    /// other signature hash flags protect the outputs.
    hash_none = bit_right<uint8_t>(hash_none_bit),

    /// The only output signed is the one corresponding to this input (the
    /// output with the same output index number as this input), ensuring
    /// nobody can change your part of the transaction but allowing other
    /// signers to change their part of the transaction. The corresponding
    /// output must exist or the value '1' will be signed, breaking the
    /// security scheme. This input, as well as other inputs, are included
    /// in the signature. The sequence numbers of other inputs are not
    /// included in the signature, and can be updated.
    hash_single = bit_or<uint8_t>(hash_all, hash_none),

    /// The above types can be modified with this flag, creating three new
    /// combined types.
    anyone_can_pay = bit_right<uint8_t>(anyone_can_pay_bit),

    /// Signs all of the outputs but only this one input, and it also allows
    /// anyone to add or remove other inputs, so anyone can contribute
    /// additional satoshis but they cannot change how many satoshis are
    /// sent nor where they go.
    all_anyone_can_pay = bit_or<uint8_t>(hash_all, anyone_can_pay),

    /// Signs only this one input and allows anyone to add or remove other
    /// inputs or outputs, so anyone who gets a copy of this input can spend
    /// it however they'd like.
    none_anyone_can_pay = bit_or<uint8_t>(hash_none, anyone_can_pay),

    /// Signs this one input and its corresponding output. Allows anyone to
    /// add or remove other inputs.
    single_anyone_can_pay = bit_or<uint8_t>(hash_single, anyone_can_pay),

    /// Used internally to pass results in schnorr parsing.
    invalid
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
