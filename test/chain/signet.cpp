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
#include "../mocks/blocks.hpp"

BOOST_AUTO_TEST_SUITE(signet_tests)

using namespace system::chain;

class signet_accessor
  : public signet
{
public:
    using signet::get_solution;
    using signet::to_spend;
    using signet::to_sign;
};

static const script true_challenge{ operations{ { opcode::push_positive_1 } } };

static transaction commitment_coinbase(const data_chunk& extra) NOEXCEPT
{
    const script commitment{ operations{ { opcode::op_return }, { base16_chunk("aa21a9ed0000000000000000000000000000000000000000000000000000000000000000"), false }, { extra, false } } };
    const inputs ins{ input{ point{}, script{}, 0xffffffff } };
    const outputs outs{ output{ 0, commitment } };
    return transaction{ 1, ins, outs, 0 };
}

static bool verify_block(const block& instance, const script& challenge) NOEXCEPT
{
    return signet::verify(instance.header(), *instance.transactions_ptr()->front(), instance.transaction_hashes(false), challenge);
}

static bool verify_coinbase(const transaction& coinbase, const script& challenge) NOEXCEPT
{
    return signet::verify(header{}, coinbase, hashes{ coinbase.hash(false) }, challenge);
}

// verify

BOOST_AUTO_TEST_CASE(signet__verify__signet1__true)
{
    BOOST_REQUIRE(verify_block(test::signet1, test::signet_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__signet100003__true)
{
    BOOST_REQUIRE(verify_block(test::signet100003, test::signet_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__other_challenge__false)
{
    const script challenge{ operations{ { opcode::push_size_0 } } };
    BOOST_REQUIRE(!verify_block(test::signet1, challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__changed_timestamp__false)
{
    const auto& head = test::signet1.header();
    const header changed{ head.version(), head.previous_block_hash(), head.merkle_root(), add1(head.timestamp()), head.bits(), head.nonce() };
    const block instance{ to_shared(changed), test::signet1.transactions_ptr() };
    BOOST_REQUIRE(!verify_block(instance, test::signet_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__no_txids__false)
{
    const auto coinbase = commitment_coinbase(base16_chunk("00"));
    BOOST_REQUIRE(!signet::verify(header{}, coinbase, {}, true_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__no_witness_commitment__false)
{
    const inputs ins{ input{ point{}, script{}, 0xffffffff } };
    const transaction coinbase{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    BOOST_REQUIRE(!verify_coinbase(coinbase, true_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__true_challenge_no_solution__true)
{
    BOOST_REQUIRE(verify_coinbase(commitment_coinbase(base16_chunk("00")), true_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__signet_challenge_no_solution__false)
{
    BOOST_REQUIRE(!verify_coinbase(commitment_coinbase(base16_chunk("00")), test::signet_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__true_challenge_empty_solution__true)
{
    BOOST_REQUIRE(verify_coinbase(commitment_coinbase(base16_chunk("ecc7daa20000")), true_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__extraneous_solution_data__false)
{
    BOOST_REQUIRE(!verify_coinbase(commitment_coinbase(base16_chunk("ecc7daa20000ff")), true_challenge));
}

BOOST_AUTO_TEST_CASE(signet__verify__truncated_solution__false)
{
    BOOST_REQUIRE(!verify_coinbase(commitment_coinbase(base16_chunk("ecc7daa201")), true_challenge));
}

// get_solution

BOOST_AUTO_TEST_CASE(signet__get_solution__signet1_commitment__expected_nominal_head)
{
    const script commitment{ base16_chunk("6a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf94c4fecc7daa2490047304402205e423a8754336ca99dbe16509b877ef1bf98d008836c725005b3c787c41ebe46022047246e4467ad7cc7f1ad98662afcaf14c115e0095a227c7b05c5182591c23e7e0100"), false };
    data_chunk solution{};
    data_chunk cleared{};
    BOOST_REQUIRE(signet_accessor::get_solution(solution, cleared, commitment));
    BOOST_REQUIRE_EQUAL(encode_base16(solution), "490047304402205e423a8754336ca99dbe16509b877ef1bf98d008836c725005b3c787c41ebe46022047246e4467ad7cc7f1ad98662afcaf14c115e0095a227c7b05c5182591c23e7e0100");
    BOOST_REQUIRE_EQUAL(encode_base16(cleared), "6a24aa21a9ede2f61c3f71d1defd3fa999dfa36953755c690689799962b48bebd836974e8cf904ecc7daa2");
}

BOOST_AUTO_TEST_CASE(signet__get_solution__head_only__false)
{
    const script commitment{ base16_chunk("6a04ecc7daa2"), false };
    data_chunk solution{};
    data_chunk cleared{};
    BOOST_REQUIRE(!signet_accessor::get_solution(solution, cleared, commitment));
    BOOST_REQUIRE(solution.empty());
}

BOOST_AUTO_TEST_CASE(signet__get_solution__second_head__first_extracted)
{
    const script commitment{ base16_chunk("6a05ecc7daa2aa05ecc7daa2bb"), false };
    data_chunk solution{};
    data_chunk cleared{};
    BOOST_REQUIRE(signet_accessor::get_solution(solution, cleared, commitment));
    BOOST_REQUIRE_EQUAL(encode_base16(solution), "aa");
    BOOST_REQUIRE_EQUAL(encode_base16(cleared), "6a04ecc7daa205ecc7daa2bb");
}

BOOST_AUTO_TEST_CASE(signet__get_solution__underflow_after_head__truncated)
{
    const script commitment{ base16_chunk("6a05ecc7daa2aa4c"), false };
    data_chunk solution{};
    data_chunk cleared{};
    BOOST_REQUIRE(signet_accessor::get_solution(solution, cleared, commitment));
    BOOST_REQUIRE_EQUAL(encode_base16(cleared), "6a04ecc7daa2");
}

// to_spend

BOOST_AUTO_TEST_CASE(signet__to_spend__default_header__expected)
{
    const auto spend = signet_accessor::to_spend(header{}, null_hash, test::signet_challenge);
    BOOST_REQUIRE(spend.is_valid());

    const auto& in = *spend.inputs_ptr()->front();
    BOOST_REQUIRE_EQUAL(encode_base16(in.script().to_data(false)), "0048000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    BOOST_REQUIRE(in.point().is_null());
    BOOST_REQUIRE_EQUAL(in.sequence(), 0u);
    BOOST_REQUIRE(spend.outputs_ptr()->front()->script() == test::signet_challenge);
    BOOST_REQUIRE_EQUAL(spend.outputs_ptr()->front()->value(), 0u);
    BOOST_REQUIRE_EQUAL(spend.version(), 0u);
    BOOST_REQUIRE_EQUAL(spend.locktime(), 0u);
}

// to_sign

BOOST_AUTO_TEST_CASE(signet__to_sign__signet1__expected)
{
    const auto& instance = test::signet1;
    const auto spend = signet_accessor::to_sign(instance.header(), *instance.transactions_ptr()->front(), instance.transaction_hashes(false), test::signet_challenge);
    BOOST_REQUIRE(spend.is_valid());

    const auto& in = *spend.inputs_ptr()->front();
    BOOST_REQUIRE_EQUAL(encode_base16(in.script().to_data(false)), "0047304402205e423a8754336ca99dbe16509b877ef1bf98d008836c725005b3c787c41ebe46022047246e4467ad7cc7f1ad98662afcaf14c115e0095a227c7b05c5182591c23e7e01");
    BOOST_REQUIRE(in.witness().stack().empty());
    BOOST_REQUIRE_EQUAL(in.sequence(), 0u);
    BOOST_REQUIRE(in.prevout->script() == test::signet_challenge);
    BOOST_REQUIRE_EQUAL(in.prevout->value(), 0u);
    BOOST_REQUIRE_EQUAL(spend.version(), 0u);
    BOOST_REQUIRE_EQUAL(spend.locktime(), 0u);
    BOOST_REQUIRE_EQUAL(encode_base16(spend.outputs_ptr()->front()->script().to_data(false)), "6a");
}

BOOST_AUTO_TEST_CASE(signet__to_sign__no_witness_commitment__invalid)
{
    const inputs ins{ input{ point{}, script{}, 0xffffffff } };
    const transaction coinbase{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    BOOST_REQUIRE(!signet_accessor::to_sign(header{}, coinbase, hashes{ coinbase.hash(false) }, true_challenge).is_valid());
}

BOOST_AUTO_TEST_SUITE_END()
