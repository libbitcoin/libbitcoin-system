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
#include <bitcoin/system/error/block_error_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(block_error)
{
    { block_success, "block success" },

    // check header
    { invalid_proof_of_work, "proof of work invalid" },
    { futuristic_timestamp, "timestamp too far in the future" },

    // accept header
    { checkpoint_conflict, "block hash rejected by checkpoint" },
    { invalid_block_version, "block version rejected at current height" },
    { timestamp_too_early, "block timestamp is too early" },
    { incorrect_proof_of_work, "proof of work does not match bits field" },

    // confirm header
    { orphan_block, "block parent is missing" },

    // check block
    { block_size_limit, "block size limit exceeded" },
    { empty_block, "block has no transactions" },
    { first_not_coinbase, "first transaction not a coinbase" },
    { extra_coinbases, "more than one coinbase" },
    { internal_duplicate, "matching transaction hashes in block" },
    { block_internal_double_spend, "double spend internal to block" },
    { forward_reference, "transactions out of order" },
    { invalid_transaction_commitment, "invalid transaction commitment" },
    { block_legacy_sigop_limit, "too many block legacy signature operations" },

    // accept block
    { block_non_final, "block contains a non-final transaction" },
    { coinbase_height_mismatch, "block height mismatch in coinbase" },
    { coinbase_value_limit, "coinbase value too high" },
    { block_sigop_limit, "too many block signature operations" },
    { invalid_witness_commitment, "invalid witness commitment" },
    { block_weight_limit, "block weight limit exceeded" },
    { temporary_hash_limit, "block contains too many hashes" },
    { unspent_coinbase_collision, "unspent coinbase collision" }
};

DEFINE_ERROR_T_CATEGORY(block_error, "block", "block code")

} // namespace error
} // namespace system
} // namespace libbitcoin
