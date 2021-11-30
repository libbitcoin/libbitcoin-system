/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ERROR_BLOCK_ERROR_T_HPP
#define LIBBITCOIN_SYSTEM_ERROR_BLOCK_ERROR_T_HPP

#include <cstdint>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

// Block sequence is limited to 0xff for single byte store encoding.
// With the uint8_t domain specified the compiler enforces this guard.

enum block_error_t : uint8_t
{
    block_success = 0,

    // check header
    invalid_proof_of_work,
    futuristic_timestamp,

    // TODO: order these.

    // accept header
    checkpoints_failed,
    invalid_block_version,
    timestamp_too_early,
    incorrect_proof_of_work,

    // check block
    block_size_limit,
    empty_block,
    first_not_coinbase,
    extra_coinbases,
    internal_duplicate,
    block_internal_double_spend,
    forward_reference,
    merkle_mismatch,
    block_legacy_sigop_limit,

    // accept block
    block_non_final,
    coinbase_height_mismatch,
    coinbase_value_limit,
    block_sigop_limit,
    invalid_witness_commitment,
    block_weight_limit,
    temporary_hash_limit,

    // confirm block
    unspent_coinbase_collision,

    // not currently used
    block_error_last
};

DECLARE_ERROR_T_CODE_CATEGORY(block_error);

} // namespace error
} // namespace system
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::system::error::block_error)

#endif
