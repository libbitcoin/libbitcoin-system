/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <sstream>
#include "bitcoin/system/chain/enums/coverage.hpp"
#include "bitcoin/system/chain/enums/forks.hpp"
#include "bitcoin/system/chain/enums/script_version.hpp"
#include "bitcoin/system/chain/script.hpp"
#include "bitcoin/system/chain/transaction.hpp"
#include "bitcoin/system/crypto/secp256k1.hpp"
#include "bitcoin/system/error/op_error_t.hpp"
#include "bitcoin/system/error/transaction_error_t.hpp"
#include "bitcoin/system/radix/base_16.hpp"
#include "script.hpp"

BOOST_AUTO_TEST_SUITE(transaction_sign_tests)

using namespace system::chain;
using namespace system::machine;

// Test helpers.
// -----------------------------------------------------------------------------

class transaction_accessor
  : public transaction
{
public:
    using transaction::transaction;

    code connect(const context& state) const NOEXCEPT
    {
        return transaction::connect(state);
    }

    code connect(const context& state, uint32_t index) const NOEXCEPT
    {
        return interpreter<contiguous_stack>::connect(state, *this, index);
    }
};

transaction_accessor test_tx(const script_test& test)
{
    const script in{ test.input };
    const script out{ test.output };

    if (!in.is_valid() || !out.is_valid())
        return {};

    const transaction_accessor tx
    {
        test.version,
        inputs
        {
            {
                point{},
                in,
                test.input_sequence
            }
        },
        outputs{},
        test.locktime
    };

    tx.inputs_ptr()->front()->prevout.reset(new prevout{ 0u, out });
    return tx;
}

std::string test_name(const script_test& test)
{
    std::stringstream out;
    out << "input: \"" << test.input << "\" "
        << "prevout: \"" << test.output << "\" "
        << "("
            << test.input_sequence << ", "
            << test.locktime << ", "
            << test.version
        << ") "
        << "name: " << test.description;
    return out.str();
}

// The following tests are derived from the test/chain/transaction.cpp
// create_endorsement tests

// Seed: d31e623e6dff00d5c0fd3aff010b80cc63787a63cbd564cd
// key: b1a44402170506553b367a6496cc7650ca1849a46eb4e01f5e4f5b72ef5e17de
// public_key: 02b59709bc5c29246bd94d48e90dde62442e97d1ccd9a2315b6303c82b07aa4412
// public_key hash: ca041a6aed6e8ab099cd75d65f8ef99e669a2d95
BOOST_AUTO_TEST_CASE(transaction__generate_signatures__single_input_single_output__expected)
{
    const ec_secret secret_key = base16_array("b1a44402170506553b367a6496cc7650ca1849a46eb4e01f5e4f5b72ef5e17de");
    const chain::transaction previous_transaction
    {
        1,
        chain::inputs
        {
            chain::input
            {
                
                // coinbase
                chain::point{ null_hash, point::null_index },
                chain::script{},
                1
            },
        },
        chain::outputs
        {
            chain::output
            {
                100,
                chain::script{"dup hash160 [ca041a6aed6e8ab099cd75d65f8ef99e669a2d95] equalverify checksig"}
            },
        },
        144
    };

    const chain::transaction spending_transaction
    {
        1,
        chain::inputs
        {
            chain::input
            {
                
                // coinbase
                chain::point{ previous_transaction.hash(false), 0u },
                // The signature is the secret key with sighash appended at the end [<secret_key><sighash_flags>]
                chain::script{"[b1a44402170506553b367a6496cc7650ca1849a46eb4e01f5e4f5b72ef5e17de00] [02b59709bc5c29246bd94d48e90dde62442e97d1ccd9a2315b6303c82b07aa4412]"},
                1
            },
        },
        chain::outputs
        {
            chain::output
            {
                90, // leave 10 as fee, spend back to d01 key hash
                chain::script{"dup hash160 [d01453c0b2e4abaa6589fd156da42e875c413539] equalverify checksig"}
            },
        },
        144
    };
    
    
    BOOST_REQUIRE(previous_transaction.is_valid());
    BOOST_REQUIRE(spending_transaction.is_valid());

    const auto prevout_script = previous_transaction.outputs_ptr()->front()->script();

    spending_transaction.inputs_ptr()->front()->prevout.reset(new prevout{0u, prevout_script});
    auto signatures = spending_transaction.generate_signatures(0u, {forks::no_rules,0});

    BOOST_REQUIRE_EQUAL(encode_base16(signatures.front()), "3044022052be662338ea349319271ff945f09b006a25e71aa6bdee421ef064d6a701af8102205a07aead4ab5b99f09ac69fd4352cbfeb52105b2dfc9f333a407925de7fdefb3");

    // Now use the signature generated and verify a transaction with the signature
    const chain::transaction transaction_with_signature
    {
        1,
        chain::inputs
        {
            chain::input
            {
                // coinbase
                chain::point{ previous_transaction.hash(false), 0u },
                // The signature is the secret key with sighash flags appended at the end [<secret_key><sighash_flags>]
                chain::script{"[3044022052be662338ea349319271ff945f09b006a25e71aa6bdee421ef064d6a701af8102205a07aead4ab5b99f09ac69fd4352cbfeb52105b2dfc9f333a407925de7fdefb300] [02b59709bc5c29246bd94d48e90dde62442e97d1ccd9a2315b6303c82b07aa4412]"},
                1
            },
        },
        chain::outputs
        {
            chain::output
            {
                90, // leave 10 as fee, spend back to d01 key hash
                chain::script{"dup hash160 [d01453c0b2e4abaa6589fd156da42e875c413539] equalverify checksig"}
            },
        },
        144
    };

    transaction_with_signature.inputs_ptr()->front()->prevout.reset(new prevout{0u, prevout_script});
    BOOST_REQUIRE_EQUAL(transaction_with_signature.connect({forks::no_rules, 0}), error::transaction_success);
}

BOOST_AUTO_TEST_SUITE_END()
