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

BOOST_AUTO_TEST_CASE(transaction__sign_transaction__single_input_single_output__expected)
{
  // create a transaction with p2pkh output
  // 1. Create a key pair
  // 2. Create previous transaction
  //    - Input: coinbase.
  //    - Output: p2pkh of the pk above
  // 3. Create spending transaction
  //    - Input: previous tx, index 0;
  //    - Output: p2pkh of the pk above
  // Then sign the spending transaction using generate_signatures
  //    - Pass secret key
  //    - Input program is obtained by spending_transaction->input->script
  //    - Prevout program is obtained by spending_transaction->input->prevout->script

  // Seed: d31e623e6dff00d5c0fd3aff010b80cc63787a63cbd564cd
  // key: b1a44402170506553b367a6496cc7650ca1849a46eb4e01f5e4f5b72ef5e17de
  // public_key: 02b59709bc5c29246bd94d48e90dde62442e97d1ccd9a2315b6303c82b07aa4412
  // public_key hash: ca041a6aed6e8ab099cd75d65f8ef99e669a2d95
    
    const ec_secret secret_key = base16_array("b1a44402170506553b367a6496cc7650ca1849a46eb4e01f5e4f5b72ef5e17de");
    // ec_compressed public_point;
    // const auto public_key = secret_to_public(public_point, secret_key);

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
                // Use the private key instead of the public key.
                // Use a dummy place holder signature
                chain::script{"[000000000001] [b1a44402170506553b367a6496cc7650ca1849a46eb4e01f5e4f5b72ef5e17de]"},
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

    // std::cerr << " prevout a " << test_tx.inputs_ptr()->front()->prevout->script().to_string(forks::no_rules) << std::endl;

    spending_transaction.inputs_ptr()->front()->prevout.reset(new prevout{0u, prevout_script});

    // std::cerr << " prevout b " << test_tx.inputs_ptr()->front()->prevout->script().to_string(forks::no_rules) << std::endl;

    // std::cerr << encode_base16(test_tx.to_data(false)) << std::endl;
    // const ec_secret secret = base16_hash("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");
    std::cerr << "Spending transaction inputs size:: " <<
        spending_transaction.inputs_ptr()->size() << std::endl;
    std::cerr << "Spending transaction input script:: " <<
        spending_transaction.inputs_ptr()->front()->script().to_string(forks::no_rules) << std::endl;
    std::cerr << "Spending transaction OUTPUT size:: " <<
        spending_transaction.outputs_ptr()->size() << std::endl;
    std::cerr << "Spending transaction OUTPUT script:: " <<
        spending_transaction.outputs_ptr()->front()->script().to_string(forks::all_rules) << std::endl;
    auto signatures = spending_transaction.generate_signatures(0u, secret_key, coverage::hash_all, {forks::no_rules,0});

    // // const auto expected = base16_chunk("3045022100e428d3cc67a724cb6cfe8634aa299e58f189d9c46c02641e936c40cc16c7e8ed0220083949910fe999c21734a1f33e42fca15fb463ea2e08f0a1bccd952aacaadbb801");
    // BOOST_REQUIRE_EQUAL(signatures.size(), 0);
}


// // bip143 test cases.
// // ----------------------------------------------------------------------------

// BOOST_AUTO_TEST_CASE(script__verify__bip143_native_p2wpkh_tx__success)
// {
//     const auto decoded_tx = base16_chunk("01000000000102fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f00000000494830450221008b9d1dc26ba6a9cb62127b02742fa9d754cd3bebf337f7a55d114c8e5cdd30be022040529b194ba3f9281a99f2b1c0a19c0489bc22ede944ccf4ecbab4cc618ef3ed01eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac000247304402203609e17b84f6a7d30c80bfa610b5b4542f32a8a0d5447a12fb1366d7f01cc44a0220573a954c4518331561406f90300e8f3358f51928d43c212a8caed02de67eebee0121025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee635711000000");
//     const transaction_accessor tx(decoded_tx, true);
//     BOOST_REQUIRE(tx.is_valid());
//     BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 2u);
//     BOOST_REQUIRE((*tx.inputs_ptr())[0]->witness().stack().empty());
//     BOOST_REQUIRE(!(*tx.inputs_ptr())[1]->witness().stack().empty());

//     constexpr auto value0 = 625000000u;
//     (*tx.inputs_ptr())[0]->prevout.reset(new prevout{ value0, { base16_chunk("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac"), false } });

//     constexpr auto value1 = 600000000u;
//     (*tx.inputs_ptr())[1]->prevout.reset(new prevout{ value1, { base16_chunk("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"), false } });

//     // ordinary P2PK (no rules required).
//     BOOST_REQUIRE_EQUAL(tx.connect({ forks::no_rules }, 0), error::script_success);

//     // P2WPKH witness program.
//     BOOST_REQUIRE_EQUAL(tx.connect({ forks::bip141_rule | forks::bip143_rule }, 1), error::script_success);

//     // Other scenarios:

//     // extra rules (okay).
//     BOOST_REQUIRE_EQUAL(tx.connect({ forks::bip141_rule | forks::bip143_rule }, 0), error::script_success);

//     // missing bip143 (invalid sighash).
//     BOOST_REQUIRE_EQUAL(tx.connect({ forks::bip141_rule }, 1), error::stack_false);

//     // missing bip141 (witness not allowed).
//     BOOST_REQUIRE_EQUAL(tx.connect({ forks::bip143_rule }, 1), error::unexpected_witness);

//     // missing bip141 (witness not allowed).
//     BOOST_REQUIRE_EQUAL(tx.connect({ forks::no_rules }, 1), error::unexpected_witness);
// }


BOOST_AUTO_TEST_SUITE_END()
