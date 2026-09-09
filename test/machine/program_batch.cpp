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
#include "accessor.hpp"

BOOST_AUTO_TEST_SUITE(program_batch_tests)

using namespace system::chain;
using namespace system::machine;

// Batch capture fallbacks (protected surface).
// ----------------------------------------------------------------------------
// Batching is gated on the running script being an output script of a
// batchable pattern; every other route falls through to inline verification.

using batch_accessor = interpreter_accessor<contiguous_stack>;

constexpr auto batch_secret = base16_array("0000000000000000000000000000000000000000000000000000000000000001");

static data_chunk batch_key() NOEXCEPT
{
    ec_compressed public_key{};
    secret_to_public(public_key, batch_secret);
    return to_chunk(public_key);
}

static script batch_p2pk() NOEXCEPT
{
    const operations ops
    {
        operation{ batch_key(), false },
        operation{ opcode::checksig }
    };
    return script{ ops };
}

// The key is supplied by the input, so its size is not constrained here.
static script batch_p2kh() NOEXCEPT
{
    const operations ops
    {
        operation{ opcode::dup },
        operation{ opcode::hash160 },
        operation{ data_chunk(short_hash_size, 0x42), true },
        operation{ opcode::equalverify },
        operation{ opcode::checksig }
    };

    return script{ ops };
}

static script batch_multisig() NOEXCEPT
{
    const auto positive1 = operation::opcode_from_positive(1_u8);
    const operations ops
    {
        operation{ positive1 },
        operation{ batch_key(), false },
        operation{ positive1 },
        operation{ opcode::checkmultisig }
    };

    return script{ ops };
}

// A point of neither compressed nor uncompressed size verifies inline.
BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__uncompressible_point__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2kh()) };

    const data_chunk point(short_hash_size, 0x00);
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!out.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(!capture.batched.load());
}

BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__unbatchable_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(script{ operations{ operation{ opcode::checksig } } }) };

    const auto point = batch_key();
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!out.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(!capture.batched.load());
}

// An input script is never batchable.
BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__input_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(batch_p2pk(), max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };

    const auto point = batch_key();
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!in.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(!capture.batched.load());
}

BOOST_AUTO_TEST_CASE(program__verify_schnorr_signature__unbatchable_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2pk()) };

    const data_chunk point(ec_xonly_size, 0x02);
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!out.verify_schnorr_signature(point, hash, signature));
    BOOST_REQUIRE(!capture.batched.load());
}

// A group must carry at least one signature.
BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__no_endorsements__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{};
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__empty_endorsement__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const data_chunk empty{};
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ empty } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

// A decodable endorsement, so the uniform sighash check is reached.
static data_chunk batch_endorsement(uint8_t sighash) NOEXCEPT
{
    ec_signature signature{};
    ecdsa::sign(signature, batch_secret, hash_digest{});
    der_signature der{};
    ecdsa::encode_signature(der, signature);

    auto endorsement = to_chunk(der);
    endorsement.push_back(sighash);
    return endorsement;
}

// One digest per group, so the sighash byte must be uniform.
BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__mixed_sighash__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const auto all = batch_endorsement(coverage::hash_all);
    const auto none = batch_endorsement(coverage::hash_none);
    const chunk_xptrs points{ chunk_xptr{ key }, chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ all }, chunk_xptr{ none } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__unbatchable_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2pk()) };

    const auto key = batch_key();
    const auto endorsement = batch_endorsement(coverage::hash_all);
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ endorsement } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__undecodable_endorsement__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const data_chunk garbage(ec_signature_size, 0xff);
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ garbage } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

// Threshold capture.
// ----------------------------------------------------------------------------
// A satisfiable terminal comparison opens a row cursor at the first sigop,
// and each subsequent sigop streams to it.

constexpr auto batch_secret2 = base16_array("0000000000000000000000000000000000000000000000000000000000000002");

static ec_xonly batch_xonly(const ec_secret& secret) NOEXCEPT
{
    ec_compressed public_key{};
    secret_to_public(public_key, secret);
    const auto xonly = std::next(public_key.data());
    return unsafe_array_cast<uint8_t, ec_xonly_size>(xonly);
}

static ec_signature batch_schnorr(const ec_secret& secret,
    const hash_digest& hash) NOEXCEPT
{
    ec_signature signature{};
    schnorr::sign(signature, secret, hash, {});
    return signature;
}

// <key0> CHECKSIG <key1> CHECKSIGADD 2 NUMEQUAL
static script batch_threshold() NOEXCEPT
{
    const operations ops
    {
        operation{ to_chunk(batch_xonly(batch_secret)), false },
        operation{ opcode::checksig },
        operation{ to_chunk(batch_xonly(batch_secret2)), false },
        operation{ opcode::checksigadd },
        operation{ opcode::push_positive_2 },
        operation{ opcode::numequal }
    };

    return script{ ops };
}

// An input script is never batchable.
BOOST_AUTO_TEST_CASE(program__verify_schnorr_signature__threshold_input_script__verified_inline)
{
    auto& rows = chain::signatures::schnorr_rows();
    rows.clear();

    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(batch_threshold(), max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };

    const hash_digest hash{};
    const auto point = to_chunk(batch_xonly(batch_secret));
    const auto signature = batch_schnorr(batch_secret, hash);
    BOOST_REQUIRE(in.verify_schnorr_signature(point, hash, signature));
    BOOST_REQUIRE(rows.empty());
    BOOST_REQUIRE(!capture.batched.load());
}

BOOST_AUTO_TEST_CASE(program__verify_schnorr_signature__threshold_output_script__batched_and_verifies)
{
    auto& rows = chain::signatures::schnorr_rows();
    rows.clear();

    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    batch_accessor out{ in, to_shared<script>(batch_threshold()) };

    // The unevaluated signature on the stack implies two expected sigops.
    out.push_chunk(data_chunk{ 0x42_u8 });

    const hash_digest hash{};
    const auto point0 = to_chunk(batch_xonly(batch_secret));
    const auto point1 = to_chunk(batch_xonly(batch_secret2));
    const auto signature0 = batch_schnorr(batch_secret, hash);
    const auto signature1 = batch_schnorr(batch_secret2, hash);
    BOOST_REQUIRE(out.verify_schnorr_signature(point0, hash, signature0));
    BOOST_REQUIRE(out.verify_schnorr_signature(point1, hash, signature1));
    BOOST_REQUIRE(capture.batched.load());
    BOOST_REQUIRE_EQUAL(rows.rows().size(), two);
    BOOST_REQUIRE_EQUAL(rows.thresholds(), two);
    BOOST_REQUIRE(rows.verify());
    rows.clear();
}

// Capture fabricates sigop success, deferring the verdict to the accumulator.
BOOST_AUTO_TEST_CASE(program__verify_schnorr_signature__threshold_wrong_signature__batched_but_fails_verify)
{
    auto& rows = chain::signatures::schnorr_rows();
    rows.clear();

    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    batch_accessor out{ in, to_shared<script>(batch_threshold()) };
    out.push_chunk(data_chunk{ 0x42_u8 });

    const hash_digest hash{};
    const auto point0 = to_chunk(batch_xonly(batch_secret));
    const auto point1 = to_chunk(batch_xonly(batch_secret2));
    const auto signature0 = batch_schnorr(batch_secret, hash);
    BOOST_REQUIRE(out.verify_schnorr_signature(point0, hash, signature0));
    BOOST_REQUIRE(out.verify_schnorr_signature(point1, hash, signature0));
    BOOST_REQUIRE(capture.batched.load());
    BOOST_REQUIRE_EQUAL(rows.rows().size(), two);
    BOOST_REQUIRE(!rows.verify());
    rows.clear();
}

// Capture.
// ----------------------------------------------------------------------------
// A batchable pattern captures and fabricates success, deferring the verdict.

static script batch_key_path() NOEXCEPT
{
    const operations ops
    {
        operation{ to_chunk(batch_xonly(batch_secret)), false },
        operation{ opcode::checksig }
    };

    return script{ ops };
}

BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__batchable_script__captured)
{
    auto& rows = chain::signatures::ecdsa_rows();
    rows.clear();

    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2pk()) };

    const auto point = batch_key();
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(out.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(capture.batched.load());
    BOOST_REQUIRE_EQUAL(rows.groups(), one);
    rows.clear();
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__batchable_group__captured)
{
    auto& rows = chain::signatures::ecdsa_rows();
    rows.clear();

    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const auto endorsement = batch_endorsement(coverage::hash_all);
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ endorsement } };
    BOOST_REQUIRE(out.try_batch_multisig_verification(points, endorsements));
    BOOST_REQUIRE(capture.batched.load());
    BOOST_REQUIRE_EQUAL(rows.groups(), one);
    rows.clear();
}

// The key path pattern is a one of one threshold, so the stack must exceed the
// threshold minimum for the single sigop capture to be reached.
BOOST_AUTO_TEST_CASE(program__verify_schnorr_signature__batchable_script__captured)
{
    auto& rows = chain::signatures::schnorr_rows();
    rows.clear();

    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    batch_accessor out{ in, to_shared<script>(batch_key_path()) };
    out.push_chunk(data_chunk{ 0x42_u8 });

    const hash_digest hash{};
    const auto point = to_chunk(batch_xonly(batch_secret));
    const auto signature = batch_schnorr(batch_secret, hash);
    BOOST_REQUIRE(out.verify_schnorr_signature(point, hash, signature));
    BOOST_REQUIRE(capture.batched.load());
    BOOST_REQUIRE_EQUAL(rows.rows().size(), one);
    BOOST_REQUIRE(rows.verify());
    rows.clear();
}

BOOST_AUTO_TEST_SUITE_END()

