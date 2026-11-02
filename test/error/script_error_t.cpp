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

BC_AUTO_TEST_SUITE(script_error_t_tests)

// These test std::error_code equality operator overrides.

BC_AUTO_TEST_CASE(script_error_t__code__script_success__false_expected_message)
{
    constexpr auto value = error::script_success;
    const auto ec = code(value);
    BC_REQUIRE(!ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "script success");
}

BC_AUTO_TEST_CASE(script_error_t__code__inputs_overflow__true_expected_message)
{
    constexpr auto value = error::inputs_overflow;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "index exceeds inputs");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_script__true_expected_message)
{
    constexpr auto value = error::invalid_script;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid script");
}

BC_AUTO_TEST_CASE(script_error_t__code__prefail_script__true_expected_message)
{
    constexpr auto value = error::prefail_script;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "script contains invalid opcode");
}

BC_AUTO_TEST_CASE(script_error_t__code__prevalid_script__true_expected_message)
{
    constexpr auto value = error::prevalid_script;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "script contains success opcode");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_script_size__true_expected_message)
{
    constexpr auto value = error::invalid_script_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid script size");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_push_data_size__true_expected_message)
{
    constexpr auto value = error::invalid_push_data_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid push data size");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_operation_count__true_expected_message)
{
    constexpr auto value = error::invalid_operation_count;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid operation count");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_stack_size__true_expected_message)
{
    constexpr auto value = error::invalid_stack_size;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid stack size");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_stack_scope__true_expected_message)
{
    constexpr auto value = error::invalid_stack_scope;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid stack scope");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_script_embed__true_expected_message)
{
    constexpr auto value = error::invalid_script_embed;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid script embed");
}

BC_AUTO_TEST_CASE(script_error_t__code__unversioned_script__true_expected_message)
{
    constexpr auto value = error::unversioned_script;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "unversioned script");
}

BC_AUTO_TEST_CASE(script_error_t__code__unexpected_witness__true_expected_message)
{
    constexpr auto value = error::unexpected_witness;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "unexpected witness");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_witness__true_expected_message)
{
    constexpr auto value = error::invalid_witness;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid witness");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_witness_stack__true_expected_message)
{
    constexpr auto value = error::invalid_witness_stack;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid witness stack");
}

BC_AUTO_TEST_CASE(script_error_t__code__invalid_commitment__true_expected_message)
{
    constexpr auto value = error::invalid_commitment;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid tapscript commitment");
}

BC_AUTO_TEST_CASE(script_error_t__code__dirty_witness__true_expected_message)
{
    constexpr auto value = error::dirty_witness;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "dirty witness");
}

BC_AUTO_TEST_CASE(script_error_t__code__dirty_embed__true_expected_message)
{
    constexpr auto value = error::dirty_embed;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "dirty embed");
}

BC_AUTO_TEST_CASE(script_error_t__code__stack_false__true_expected_message)
{
    constexpr auto value = error::stack_false;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "stack false");
}

BC_AUTO_TEST_SUITE_END()
