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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(op_error_t_tests)

BC_AUTO_TEST_CASE(op_error_t__code__op_success__false_expected_message)
{
    constexpr auto value = error::op_success;
    const auto ec = code(value);
    BC_REQUIRE(!ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "opcode success");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_invalid__true_expected_message)
{
    constexpr auto value = error::op_invalid;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_invalid");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_reserved__true_expected_message)
{
    constexpr auto value = error::op_reserved;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_reserved");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_push_size__true_expected_message)
{
    constexpr auto value = error::op_push_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_push_size");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_push_one_size__true_expected_message)
{
    constexpr auto value = error::op_push_one_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_push_one_size");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_push_two_size__true_expected_message)
{
    constexpr auto value = error::op_push_two_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_push_two_size");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_push_four_size__true_expected_message)
{
    constexpr auto value = error::op_push_four_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_push_four_size");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_if1__true_expected_message)
{
    constexpr auto value = error::op_if1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_if1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_if2__true_expected_message)
{
    constexpr auto value = error::op_if2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_if2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_notif1__true_expected_message)
{
    constexpr auto value = error::op_notif1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_notif1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_notif2__true_expected_message)
{
    constexpr auto value = error::op_notif2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_notif2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_else__true_expected_message)
{
    constexpr auto value = error::op_else;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_else");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_endif__true_expected_message)
{
    constexpr auto value = error::op_endif;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_endif");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_verify1__true_expected_message)
{
    constexpr auto value = error::op_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_verify2__true_expected_message)
{
    constexpr auto value = error::op_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_return__true_expected_message)
{
    constexpr auto value = error::op_return;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_return");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_to_alt_stack__true_expected_message)
{
    constexpr auto value = error::op_to_alt_stack;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_to_alt_stack");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_from_alt_stack__true_expected_message)
{
    constexpr auto value = error::op_from_alt_stack;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_from_alt_stack");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_drop2__true_expected_message)
{
    constexpr auto value = error::op_drop2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_drop2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_dup2__true_expected_message)
{
    constexpr auto value = error::op_dup2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_dup2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_dup3__true_expected_message)
{
    constexpr auto value = error::op_dup3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_dup3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_over2__true_expected_message)
{
    constexpr auto value = error::op_over2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_over2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_rot2__true_expected_message)
{
    constexpr auto value = error::op_rot2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_rot2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_swap2__true_expected_message)
{
    constexpr auto value = error::op_swap2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_swap2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_if_dup__true_expected_message)
{
    constexpr auto value = error::op_if_dup;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_if_dup");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_drop__true_expected_message)
{
    constexpr auto value = error::op_drop;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_drop");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_dup__true_expected_message)
{
    constexpr auto value = error::op_dup;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_dup");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_nip__true_expected_message)
{
    constexpr auto value = error::op_nip;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_nip");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_over__true_expected_message)
{
    constexpr auto value = error::op_over;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_over");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_pick__true_expected_message)
{
    constexpr auto value = error::op_pick;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_pick");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_roll__true_expected_message)
{
    constexpr auto value = error::op_roll;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_roll");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_rot__true_expected_message)
{
    constexpr auto value = error::op_rot;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_rot");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_swap__true_expected_message)
{
    constexpr auto value = error::op_swap;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_swap");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_tuck__true_expected_message)
{
    constexpr auto value = error::op_tuck;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_tuck");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_size__true_expected_message)
{
    constexpr auto value = error::op_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_size");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_equal__true_expected_message)
{
    constexpr auto value = error::op_equal;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_equal");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_equal_verify1__true_expected_message)
{
    constexpr auto value = error::op_equal_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_equal_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_equal_verify2__true_expected_message)
{
    constexpr auto value = error::op_equal_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_equal_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__oop_add1__true_expected_message)
{
    constexpr auto value = error::op_add1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_add1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_sub1__true_expected_message)
{
    constexpr auto value = error::op_sub1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_sub1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_negate__true_expected_message)
{
    constexpr auto value = error::op_negate;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_negate");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_abs__true_expected_message)
{
    constexpr auto value = error::op_abs;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_abs");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_not__true_expected_message)
{
    constexpr auto value = error::op_not;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_not");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_nonzero__true_expected_message)
{
    constexpr auto value = error::op_nonzero;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_nonzero");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_add__true_expected_message)
{
    constexpr auto value = error::op_add;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_add");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_sub__true_expected_message)
{
    constexpr auto value = error::op_sub;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_sub");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_bool_and__true_expected_message)
{
    constexpr auto value = error::op_bool_and;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_bool_and");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_bool_or__true_expected_message)
{
    constexpr auto value = error::op_bool_or;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_bool_or");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_num_equal__true_expected_message)
{
    constexpr auto value = error::op_num_equal;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_num_equal");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_num_equal_verify1__true_expected_message)
{
    constexpr auto value = error::op_num_equal_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_num_equal_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_num_equal_verify2__true_expected_message)
{
    constexpr auto value = error::op_num_equal_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_num_equal_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_num_not_equal__true_expected_message)
{
    constexpr auto value = error::op_num_not_equal;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_num_not_equal");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_less_than__true_expected_message)
{
    constexpr auto value = error::op_less_than;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_less_than");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_greater_than__true_expected_message)
{
    constexpr auto value = error::op_greater_than;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_greater_than");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_less_than_or_equal__true_expected_message)
{
    constexpr auto value = error::op_less_than_or_equal;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_less_than_or_equal");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_greater_than_or_equal__true_expected_message)
{
    constexpr auto value = error::op_greater_than_or_equal;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_greater_than_or_equal");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_min__true_expected_message)
{
    constexpr auto value = error::op_min;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_min");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_max__true_expected_message)
{
    constexpr auto value = error::op_max;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_max");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_within__true_expected_message)
{
    constexpr auto value = error::op_within;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_within");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_ripemd160__true_expected_message)
{
    constexpr auto value = error::op_ripemd160;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_ripemd160");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_sha1__true_expected_message)
{
    constexpr auto value = error::op_sha1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_sha1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_sha256__true_expected_message)
{
    constexpr auto value = error::op_sha256;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_sha256");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_hash160__true_expected_message)
{
    constexpr auto value = error::op_hash160;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_hash160");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_hash256__true_expected_message)
{
    constexpr auto value = error::op_hash256;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_hash256");
}

// op_check_sig/verify

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_low_stack__true_expected_message)
{
    constexpr auto value = error::op_check_sig_low_stack;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_low_stack");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_verify1__true_expected_message)
{
    constexpr auto value = error::op_check_sig_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_verify2__true_expected_message)
{
    constexpr auto value = error::op_check_sig_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_verify3__true_expected_message)
{
    constexpr auto value = error::op_check_sig_verify3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_verify3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_verify4__true_expected_message)
{
    constexpr auto value = error::op_check_sig_verify4;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_verify4");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_empty_key__true_expected_message)
{
    constexpr auto value = error::op_check_sig_empty_key;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_empty_key");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_schnorr1__true_expected_message)
{
    constexpr auto value = error::op_check_sig_schnorr1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_schnorr1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_schnorr2__true_expected_message)
{
    constexpr auto value = error::op_check_sig_schnorr2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_schnorr2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_schnorr3__true_expected_message)
{
    constexpr auto value = error::op_check_sig_schnorr3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_schnorr3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_budget__true_expected_message)
{
    constexpr auto value = error::op_check_sig_budget;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_budget");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_decode_signature__true_expected_message)
{
    constexpr auto value = error::op_check_sig_decode_signature;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_decode_signature");
}

// op_check_multisig/verify

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify1__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify2__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify3__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify4__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify4;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify4");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify5__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify5;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify5");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify6__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify6;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify6");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify7__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify7;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify7");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify8__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify8;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify8");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_verify9__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_verify9;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_verify9");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_mismatch__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_mismatch;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_mismatch");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_multisig_decode_signature__true_expected_message)
{
    constexpr auto value = error::op_check_multisig_decode_signature;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_multisig_decode_signature");
}

// op_check_locktime_verify

BC_AUTO_TEST_CASE(op_error_t__code__op_check_locktime_verify1__true_expected_message)
{
    constexpr auto value = error::op_check_locktime_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_locktime_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_locktime_verify2__true_expected_message)
{
    constexpr auto value = error::op_check_locktime_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_locktime_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_locktime_verify3__true_expected_message)
{
    constexpr auto value = error::op_check_locktime_verify3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_locktime_verify3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_locktime_verify4__true_expected_message)
{
    constexpr auto value = error::op_check_locktime_verify4;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_locktime_verify4");
}

// op_check_sequence_verify

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sequence_verify1__true_expected_message)
{
    constexpr auto value = error::op_check_sequence_verify1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sequence_verify1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sequence_verify2__true_expected_message)
{
    constexpr auto value = error::op_check_sequence_verify2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sequence_verify2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sequence_verify3__true_expected_message)
{
    constexpr auto value = error::op_check_sequence_verify3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sequence_verify3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sequence_verify4__true_expected_message)
{
    constexpr auto value = error::op_check_sequence_verify4;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sequence_verify4");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sequence_verify5__true_expected_message)
{
    constexpr auto value = error::op_check_sequence_verify5;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sequence_verify5");
}

// op_check_sig_add

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add1__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add1;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add1");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add2__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add2;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add2");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add3__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add3;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add3");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add4__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add4;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add4");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add5__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add5;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add5");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add6__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add6;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add6");
}

BC_AUTO_TEST_CASE(op_error_t__code__op_check_sig_add7__true_expected_message)
{
    constexpr auto value = error::op_check_sig_add7;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "op_check_sig_add7");
}

BC_AUTO_TEST_SUITE_END()
