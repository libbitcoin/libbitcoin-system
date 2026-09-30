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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(output_view_tests)

using namespace system::chain;

static const script p2pkh_script(base16_chunk("76a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac"), false);
static const script multisig_script(base16_chunk("5221022b78b756e2258af13779c1a1f37ea6800259716ca4b7f0b87610e0bf3ab52a012103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd2543252ae"), false);
static const script op_return_script(base16_chunk("6a0474657374"), false);

BOOST_AUTO_TEST_CASE(output_view__construct__default__invalid)
{
    const chain::view::output view{};
    BOOST_CHECK(!view.is_valid());
    BOOST_CHECK(is_null(view.data()));
}

BOOST_AUTO_TEST_CASE(output_view__construct__p2pkh__expected)
{
    const output expected{ 42, p2pkh_script };
    const auto data = expected.to_data();
    const chain::view::output view{ data.data() };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.value(), 42u);
    BOOST_CHECK_EQUAL(view.script_size(), p2pkh_script.serialized_size(false));
    BOOST_CHECK_EQUAL(view.script_data().to_chunk(), p2pkh_script.to_data(false));
    BOOST_CHECK_EQUAL(view.serialized_size(), expected.serialized_size());
    BOOST_CHECK(view.script() == p2pkh_script);
    BOOST_CHECK(*view.script_ptr() == p2pkh_script);
    BOOST_CHECK_EQUAL(view.hash(), expected.hash());
    BOOST_CHECK(!view.is_pay_op_return_pattern());
}

BOOST_AUTO_TEST_CASE(output_view__to_data__multisig__round_trips)
{
    const output expected{ 0xffffffffffffffff, multisig_script };
    const auto data = expected.to_data();
    const chain::view::output view{ data.data() };
    data_chunk out(view.serialized_size());
    stream::out::fast ostream(out);
    write::bytes::fast sink(ostream);
    view.to_data(sink);
    BOOST_CHECK_EQUAL(out, data);
}

BOOST_AUTO_TEST_CASE(output_view__is_pay_op_return_pattern__op_return__true)
{
    const output expected{ 0, op_return_script };
    const auto data = expected.to_data();
    const chain::view::output view{ data.data() };
    BOOST_CHECK(view.is_pay_op_return_pattern());
}

BOOST_AUTO_TEST_CASE(output_view__is_pay_op_return_pattern__empty__false)
{
    const output expected{ 0, script{} };
    const auto data = expected.to_data();
    const chain::view::output view{ data.data() };
    BOOST_CHECK_EQUAL(view.script_size(), 0u);
    BOOST_CHECK(!view.is_pay_op_return_pattern());
}

BOOST_AUTO_TEST_CASE(output_view__signature_operations__multisig__matches_output)
{
    const output expected{ 1, multisig_script };
    const auto data = expected.to_data();
    const chain::view::output view{ data.data() };
    BOOST_CHECK_EQUAL(view.signature_operations(false), expected.signature_operations(false));
    BOOST_CHECK_EQUAL(view.signature_operations(true), expected.signature_operations(true));
    BOOST_CHECK_EQUAL(view.signature_operations(false), 20u);
    BOOST_CHECK_EQUAL(view.signature_operations(true), 80u);
}

BOOST_AUTO_TEST_CASE(output_view__signature_operations__p2pkh__matches_output)
{
    const output expected{ 1, p2pkh_script };
    const auto data = expected.to_data();
    const chain::view::output view{ data.data() };
    BOOST_CHECK_EQUAL(view.signature_operations(false), expected.signature_operations(false));
    BOOST_CHECK_EQUAL(view.signature_operations(true), expected.signature_operations(true));
    BOOST_CHECK_EQUAL(view.signature_operations(false), 1u);
}

BOOST_AUTO_TEST_SUITE_END()
