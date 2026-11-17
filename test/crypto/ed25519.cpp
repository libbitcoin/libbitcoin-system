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

// rfc8032 section 7.1 (tests 1, 2 and 3).
const auto test1_seed = base16_array("9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");
const auto test1_expanded = base16_array("307c83864f2833cb427a2ef1c00a013cfdff2768d980c0a3a520f006904de94f9b4f0afe280b746a778684e75442502057b7473a03f08f96f5a38e9287e01f8f");
const auto test1_public = base16_array("d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a");
const auto test1_signature = base16_array("e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b");
const auto test2_seed = base16_array("4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb");
const auto test2_public = base16_array("3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c");
const auto test2_message = base16_chunk("72");
const auto test2_signature = base16_array("92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00");
const auto test3_seed = base16_array("c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7");
const auto test3_public = base16_array("fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025");
const auto test3_message = base16_chunk("af82");
const auto test3_signature = base16_array("6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a");

// Test 1 with S + l, flipped R bit, and invalid point encodings.
const auto response_plus_order = base16_array("e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901554c8c7872aa064e049dbb3013fbf29380d25bf5f0595bbe24655141438e7a101b");
const auto altered_commitment = base16_array("e4564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b");
const auto prime_y = base16_array("edffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff7f");
const auto negative_zero = base16_array("0100000000000000000000000000000000000000000000000000000000000080");
const auto off_curve = base16_array("0200000000000000000000000000000000000000000000000000000000000000");

// rend-spec-v3 A.2 key blinding (C tor vectors, as in arti keymanip.rs).
const auto tor1_seed = base16_array("26c76712d89d906e6672dafa614c42e5cb1caac8c6568e4d2493087db51f0d36");
const auto tor1_expanded = base16_array("c0a4de23cc64392d85aa1da82b3defddbea946d13bb053bf8489fa9296281f495022f1f7ec0dcf52f07d4c7965c4eaed121d5d88d0a8ff546b06116a20e97755");
const auto tor1_public = base16_array("c2247870536a192d142d056abefca68d6193158e7c1a59c1654c954eccaff894");
const auto tor1_factor = base16_array("54a513898b471d1d448a2f3c55c1de2c0ef718c447b04497eeb999ed32027823");
const auto tor1_blinded_public = base16_array("1fc1fa4465bd9d4956fdbdc9d3acb3c7019bb8d5606b951c2e1dfe0b42eaeb41");
const auto tor1_blinded_secret = base16_array("293c3acff4e902f6f63ddc5d5caa2a57e771db4f24de65d4c28df3232f47fa01171d43f24e3f53e70ec7ac280044ac77d4942dee5d6807118a59bdf3ee647e89");
const auto tor1_blinded_signature = base16_array("b5ab38f8d58e56d1d69ba96bb50566d954503c6d2445a3463e74a05d49c808a4d754e5d914c63e8b2ca53b441dbe852690edbe8a86b93263953daa178c1a6e05");
// Vectors 8 and 9 share a seed.
const auto tor8_expanded = base16_array("b003077c1e52a62308eef7950b2d532e1d4a7eea50ad22d8ac11b892851f1c40ffb9c9ff8dcd0c6c233f665a2e176324d92416bfcfcd1f787424c0c667452d86");
const auto tor8_public = base16_array("95126f14d86494020665face03f2d42ee2b312a85bc729903eb17522954a1c4a");
const auto tor8_factor = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
const auto tor8_blinded_public = base16_array("312404d06a0a9de489904b18d5233e83a50b225977fa8734f2c897a73c067952");
const auto tor8_blinded_secret = base16_array("e5cd03eb4cc456e11bc36724b558873df0045729b22d8b748360067a7770ac02053d9c00c81e1331c06ab50087be8cfc7dc11691b132614474f1aa9c2503cccd");
const auto tor9_factor = base16_array("1111111111111111111111111111111111111111111111111111111111111111");
const auto tor9_blinded_public = base16_array("952a908a4a9e0e5176a2549f8f328955aca6817a9fdc59e3acec5dec50838108");
const auto tor9_blinded_secret = base16_array("2cf7ed8b163f5af960d2fc62e1883aa422a6090736b4f18a5456ddcaf78ede0c053d9c00c81e1331c06ab50087be8cfc7dc11691b132614474f1aa9c2503cccd");
const auto hello_world = to_array("hello world");

BC_AUTO_TEST_SUITE(ed25519_tests)

// expand/public_key

BC_AUTO_TEST_CASE(ed25519__expand__rfc8032_test_1__expected)
{
    BC_REQUIRE_EQUAL(ed25519::expand(test1_seed), test1_expanded);
}

BC_AUTO_TEST_CASE(ed25519__public_key__rfc8032_test_1__expected)
{
    BC_REQUIRE_EQUAL(ed25519::public_key(test1_expanded), test1_public);
}

BC_AUTO_TEST_CASE(ed25519__public_key__rfc8032_test_2__expected)
{
    BC_REQUIRE_EQUAL(ed25519::public_key(ed25519::expand(test2_seed)), test2_public);
}

BC_AUTO_TEST_CASE(ed25519__public_key__rfc8032_test_3__expected)
{
    BC_REQUIRE_EQUAL(ed25519::public_key(ed25519::expand(test3_seed)), test3_public);
}

// sign

BC_AUTO_TEST_CASE(ed25519__sign__rfc8032_test_1__expected)
{
    BC_REQUIRE_EQUAL(ed25519::sign(test1_expanded, test1_public, {}), test1_signature);
}

BC_AUTO_TEST_CASE(ed25519__sign__rfc8032_test_2__expected)
{
    const auto secret = ed25519::expand(test2_seed);
    BC_REQUIRE_EQUAL(ed25519::sign(secret, test2_public, test2_message), test2_signature);
}

BC_AUTO_TEST_CASE(ed25519__sign__rfc8032_test_3__expected)
{
    const auto secret = ed25519::expand(test3_seed);
    BC_REQUIRE_EQUAL(ed25519::sign(secret, test3_public, test3_message), test3_signature);
}

// verify

BC_AUTO_TEST_CASE(ed25519__verify__rfc8032_test_1__true)
{
    BC_REQUIRE(ed25519::verify(test1_signature, test1_public, {}));
}

BC_AUTO_TEST_CASE(ed25519__verify__rfc8032_test_2__true)
{
    BC_REQUIRE(ed25519::verify(test2_signature, test2_public, test2_message));
}

BC_AUTO_TEST_CASE(ed25519__verify__rfc8032_test_3__true)
{
    BC_REQUIRE(ed25519::verify(test3_signature, test3_public, test3_message));
}

BC_AUTO_TEST_CASE(ed25519__verify__wrong_message__false)
{
    BC_REQUIRE(!ed25519::verify(test3_signature, test3_public, {}));
}

BC_AUTO_TEST_CASE(ed25519__verify__altered_commitment__false)
{
    BC_REQUIRE(!ed25519::verify(altered_commitment, test1_public, {}));
}

BC_AUTO_TEST_CASE(ed25519__verify__non_canonical_response__false)
{
    BC_REQUIRE(!ed25519::verify(response_plus_order, test1_public, {}));
}

BC_AUTO_TEST_CASE(ed25519__verify__non_canonical_point__false)
{
    BC_REQUIRE(!ed25519::verify(test1_signature, prime_y, {}));
}

// blind (point)

BC_AUTO_TEST_CASE(ed25519__blind__tor_vector_1_point__expected)
{
    ed25519::key out{};
    BC_REQUIRE(ed25519::blind(out, tor1_public, tor1_factor));
    BC_REQUIRE_EQUAL(out, tor1_blinded_public);
}

BC_AUTO_TEST_CASE(ed25519__blind__tor_vector_8_point_zero_factor__expected)
{
    ed25519::key out{};
    BC_REQUIRE(ed25519::blind(out, tor8_public, tor8_factor));
    BC_REQUIRE_EQUAL(out, tor8_blinded_public);
}

BC_AUTO_TEST_CASE(ed25519__blind__tor_vector_9_point__expected)
{
    ed25519::key out{};
    BC_REQUIRE(ed25519::blind(out, tor8_public, tor9_factor));
    BC_REQUIRE_EQUAL(out, tor9_blinded_public);
}

BC_AUTO_TEST_CASE(ed25519__blind__non_canonical_point__false)
{
    ed25519::key out{};
    BC_REQUIRE(!ed25519::blind(out, prime_y, tor1_factor));
}

BC_AUTO_TEST_CASE(ed25519__blind__negative_zero__false)
{
    ed25519::key out{};
    BC_REQUIRE(!ed25519::blind(out, negative_zero, tor1_factor));
}

BC_AUTO_TEST_CASE(ed25519__blind__off_curve__false)
{
    ed25519::key out{};
    BC_REQUIRE(!ed25519::blind(out, off_curve, tor1_factor));
}

// blind (secret)

BC_AUTO_TEST_CASE(ed25519__expand__tor_vector_1__expected)
{
    BC_REQUIRE_EQUAL(ed25519::expand(tor1_seed), tor1_expanded);
}

BC_AUTO_TEST_CASE(ed25519__public_key__tor_vector_1__expected)
{
    BC_REQUIRE_EQUAL(ed25519::public_key(tor1_expanded), tor1_public);
}

BC_AUTO_TEST_CASE(ed25519__blind__tor_vector_1_secret__expected)
{
    BC_REQUIRE_EQUAL(ed25519::blind(tor1_expanded, tor1_factor), tor1_blinded_secret);
}

BC_AUTO_TEST_CASE(ed25519__blind__tor_vector_8_secret_zero_factor__expected)
{
    BC_REQUIRE_EQUAL(ed25519::blind(tor8_expanded, tor8_factor), tor8_blinded_secret);
}

BC_AUTO_TEST_CASE(ed25519__blind__tor_vector_9_secret__expected)
{
    BC_REQUIRE_EQUAL(ed25519::blind(tor8_expanded, tor9_factor), tor9_blinded_secret);
}

BC_AUTO_TEST_CASE(ed25519__public_key__tor_vector_1_blinded__blinded_public)
{
    BC_REQUIRE_EQUAL(ed25519::public_key(tor1_blinded_secret), tor1_blinded_public);
}

BC_AUTO_TEST_CASE(ed25519__sign__tor_vector_1_blinded__expected)
{
    const auto signature = ed25519::sign(tor1_blinded_secret, tor1_blinded_public, hello_world);
    BC_REQUIRE_EQUAL(signature, tor1_blinded_signature);
    BC_REQUIRE(ed25519::verify(signature, tor1_blinded_public, hello_world));
}

BC_AUTO_TEST_SUITE_END()

BC_AUTO_TEST_SUITE(ed25519_ctime_tests)

// Under valgrind a secret is undefined, so a branch or memory address that
// depends on it is reported as an error, and each output is defined before it
// is compared to the expected value.

BC_AUTO_TEST_CASE(ed25519_ctime__expand__undefined_seed__expected)
{
    auto seed = test1_seed;
    classify(seed);
    auto secret = ed25519::expand(seed);
    declassify(secret);
    BC_REQUIRE_EQUAL(secret, test1_expanded);
}

BC_AUTO_TEST_CASE(ed25519_ctime__public_key__undefined_secret__expected)
{
    auto secret = test1_expanded;
    classify(secret);
    auto point = ed25519::public_key(secret);
    declassify(point);
    BC_REQUIRE_EQUAL(point, test1_public);
}

BC_AUTO_TEST_CASE(ed25519_ctime__sign__undefined_secret__expected)
{
    auto secret = test1_expanded;
    classify(secret);
    auto signature = ed25519::sign(secret, test1_public, {});
    declassify(signature);
    BC_REQUIRE_EQUAL(signature, test1_signature);
}

BC_AUTO_TEST_CASE(ed25519_ctime__blind__undefined_secret__expected)
{
    auto secret = tor1_expanded;
    classify(secret);
    auto blinded = ed25519::blind(secret, tor1_factor);
    declassify(blinded);
    BC_REQUIRE_EQUAL(blinded, tor1_blinded_secret);
}

BC_AUTO_TEST_SUITE_END()
