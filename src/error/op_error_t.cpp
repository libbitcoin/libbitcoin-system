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
#include <bitcoin/system/error/op_error_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(op_error)
{
    { op_success, "opcode success" },
    { op_not_implemented, "opcode not implemented" },
    { op_invalid, "op_invalid" },
    { op_reserved, "op_reserved" },
    { op_push_size, "op_push_size" },
    { op_push_data, "op_push_data" },
    { op_if, "op_if" },
    { op_notif, "op_notif" },
    { op_else, "op_else" },
    { op_endif, "op_endif" },
    { op_verify1, "op_verify1" },
    { op_verify2, "op_verify2" },
    { op_return, "op_return" },
    { op_to_alt_stack, "op_to_alt_stack" },
    { op_from_alt_stack, "op_from_alt_stack" },
    { op_drop2, "op_drop2" },
    { op_dup2, "op_dup2" },
    { op_dup3, "op_dup3" },
    { op_over2, "op_over2" },
    { op_rot2, "op_rot2" },
    { op_swap2, "op_swap2" },
    { op_if_dup, "op_if_dup" },
    { op_drop, "op_drop" },
    { op_dup, "op_dup" },
    { op_nip, "op_nip" },
    { op_over, "op_over" },
    { op_pick, "op_pick" },
    { op_roll, "op_roll" },
    { op_rot, "op_rot" },
    { op_swap, "op_swap" },
    { op_tuck, "op_tuck" },
    { op_size, "op_size" },
    { op_equal, "op_equal" },
    { op_equal_verify1, "op_equal_verify1" },
    { op_equal_verify2, "op_equal_verify2" },
    { op_add1, "op_add1" },
    { op_sub1, "op_sub1" },
    { op_negate, "op_negate" },
    { op_abs, "op_abs" },
    { op_not, "op_not" },
    { op_nonzero, "op_nonzero" },
    { op_add, "op_add" },
    { op_sub, "op_sub" },
    { op_bool_and, "op_bool_and" },
    { op_bool_or, "op_bool_or" },
    { op_num_equal, "op_num_equal" },
    { op_num_equal_verify1, "op_num_equal_verify1" },
    { op_num_equal_verify2, "op_num_equal_verify2" },
    { op_num_not_equal, "op_num_not_equal" },
    { op_less_than, "op_less_than" },
    { op_greater_than, "op_greater_than" },
    { op_less_than_or_equal, "op_less_than_or_equal" },
    { op_greater_than_or_equal, "op_greater_than_or_equal" },
    { op_min, "op_min" },
    { op_max, "op_max" },
    { op_within, "op_within" },
    { op_ripemd160, "op_ripemd160" },
    { op_sha1, "op_sha1" },
    { op_sha256, "op_sha256" },
    { op_hash160, "op_hash160" },
    { op_hash256, "op_hash256" },
    { op_code_seperator, "op_code_seperator" },
    { op_check_sig_verify1, "op_check_sig_verify1" },
    { op_check_sig_verify2, "op_check_sig_verify2" },
    { op_check_sig_verify3, "op_check_sig_verify3" },
    { op_check_sig_verify4, "op_check_sig_verify4" },
    { op_check_sig_verify_parse, "op_check_sig_verify_parse" },
    { op_check_sig, "op_check_sig" },
    { op_check_multisig_verify1, "op_check_multisig_verify1" },
    { op_check_multisig_verify2, "op_check_multisig_verify2" },
    { op_check_multisig_verify3, "op_check_multisig_verify3" },
    { op_check_multisig_verify4, "op_check_multisig_verify4" },
    { op_check_multisig_verify5, "op_check_multisig_verify5" },
    { op_check_multisig_verify6, "op_check_multisig_verify6" },
    { op_check_multisig_verify7, "op_check_multisig_verify7" },
    { op_check_multisig_verify8, "op_check_multisig_verify8" },
    { op_check_multisig_verify9, "op_check_multisig_verify9" },
    { op_check_multisig_verify_parse, "op_check_multisig_verify_parse" },
    { op_check_multisig, "op_check_multisig" },
    { op_check_locktime_verify1, "op_check_locktime_verify1" },
    { op_check_locktime_verify2, "op_check_locktime_verify2" },
    { op_check_locktime_verify3, "op_check_locktime_verify3" },
    { op_check_locktime_verify4, "op_check_locktime_verify4" },
    { op_check_locktime_verify5, "op_check_locktime_verify5" },
    { op_check_locktime_verify6, "op_check_locktime_verify6" },
    { op_check_sequence_verify1, "op_check_sequence_verify1" },
    { op_check_sequence_verify2, "op_check_sequence_verify2" },
    { op_check_sequence_verify3, "op_check_sequence_verify3" },
    { op_check_sequence_verify4, "op_check_sequence_verify4" },
    { op_check_sequence_verify5, "op_check_sequence_verify5" },
    { op_check_sequence_verify6, "op_check_sequence_verify6" },
    { op_check_sequence_verify7, "op_check_sequence_verify7" }
};

DEFINE_ERROR_T_CATEGORY(op_error, "op", "op code")

} // namespace error
} // namespace system
} // namespace libbitcoin
