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

BOOST_AUTO_TEST_SUITE(descriptor_tests)

using namespace bc::system::wallet;

// The genesis coinbase public key and its known p2kh form.
#define GENESIS_KEY "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"
#define GENESIS_P2KH_SCRIPT "76a91462e907b15cbf27d5425399ebf6f0fb50ebb88f1888ac"
#define GENESIS_P2KH_ADDRESS "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"

// bip32 test vector 1 public keys (m/0H and m/0H/1).
#define VECTOR1_M0H "xpub68Gmy5EdvgibQVfPdqkBBCHxA5htiqg55crXYuXoQRKfDBFA1WEjWgP6LHhwBZeNK1VTsfTFUHCdrfp1bgwQ9xv5ski8PX9rL2dZXvgGDnw"
#define VECTOR1_M0H_1 "xpub6ASuArnXKPbfEwhqN6e3mwBcDTgzisQN1wXN9BJcM47sSikHjJf3UFHKkNAWbWMiGj7Wf5uMash7SyYq527Hqck2AxYysAA7xmALppuCkwQ"

// checksum (bip380 and bitcoind vectors)

BOOST_AUTO_TEST_CASE(descriptor__to_checksum__wpkh__expected)
{
    BOOST_REQUIRE_EQUAL(descriptor::to_checksum("wpkh([d34db33f/84h/0h/0h]xpub6DJ2dNUysrn5Vt36jH2KLBT2i1auw1tTSSomg8PhqNiUtx8QX2SvC9nrHu81fT41fvDUnhMjEzQgXnQjKEu3oaqMSzhSrHMxyyoEAmUHQbY/0/*)"), "cjjspncu");
}

BOOST_AUTO_TEST_CASE(descriptor__to_checksum__sh_multi__expected)
{
    BOOST_REQUIRE_EQUAL(descriptor::to_checksum("sh(multi(2,[00000000/111'/222]xpub6ERApfZwUNrhLCkDtcHTcxd75RbzS1ed54G1LkBUHQVHQKqhMkhgbmJbZRkrgZw4koxb5JaHWkY4ALHY2grBGRjaDMzQLcgJvLJuZZvRcEL,xpub68NZiKmJWnxxS6aaHmn81bvJeTESw724CRDs6HbuccFQN9Ku14VQrADWgqbhhTHBaohPX4CjNLf9fq9MYo6oDaPPLPxSb7gwQN3ih19Zm4Y/0))"), "tjg09x5t");
}

BOOST_AUTO_TEST_CASE(descriptor__to_checksum__invalid_character__empty)
{
    BOOST_REQUIRE(descriptor::to_checksum("pk(\t)").empty());
}

// construct

BOOST_AUTO_TEST_CASE(descriptor__default_construct__always__invalid)
{
    BOOST_REQUIRE(!descriptor());
}

BOOST_AUTO_TEST_CASE(descriptor__construct__empty__invalid)
{
    BOOST_REQUIRE(!descriptor(""));
}

BOOST_AUTO_TEST_CASE(descriptor__construct__unknown_function__invalid)
{
    BOOST_REQUIRE(!descriptor("nonsense(00)"));
}

BOOST_AUTO_TEST_CASE(descriptor__construct__bad_checksum__invalid)
{
    BOOST_REQUIRE(!descriptor("raw(deadbeef)#00000000"));
}

BOOST_AUTO_TEST_CASE(descriptor__construct__valid_checksum__valid)
{
    const descriptor instance("raw(deadbeef)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(descriptor(instance.encoded()));
}

BOOST_AUTO_TEST_CASE(descriptor__encoded__round_trip__appends_checksum)
{
    const descriptor instance("raw(deadbeef)");
    BOOST_REQUIRE_EQUAL(instance.encoded(), "raw(deadbeef)#" + descriptor::to_checksum("raw(deadbeef)"));
}

// pk/pkh (genesis key)

BOOST_AUTO_TEST_CASE(descriptor__scripts__pkh_genesis__expected_script)
{
    const descriptor instance("pkh(" GENESIS_KEY ")");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(!instance.ranged());
    BOOST_REQUIRE(!instance.has_private_keys());
    BOOST_REQUIRE(instance.solvable());
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE_EQUAL(encode_base16(scripts.front().to_data(false)), GENESIS_P2KH_SCRIPT);
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__pk_genesis__expected_pattern)
{
    const descriptor instance("pk(" GENESIS_KEY ")");
    BOOST_REQUIRE(instance);
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE(chain::script::is_pay_public_key_pattern(scripts.front().ops()));
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__addr_genesis__expected_script)
{
    const descriptor instance("addr(" GENESIS_P2KH_ADDRESS ")");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(!instance.solvable());
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE_EQUAL(encode_base16(scripts.front().to_data(false)), GENESIS_P2KH_SCRIPT);
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__wpkh_uncompressed__empty)
{
    const descriptor instance("wpkh(" GENESIS_KEY ")");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.scripts(0).empty());
}

// ranged xpub derivation (bip32 vector 1)

BOOST_AUTO_TEST_CASE(descriptor__scripts__ranged_pkh__matches_hd_derivation)
{
    const descriptor instance("pkh(" VECTOR1_M0H "/1/*)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.ranged());
    BOOST_REQUIRE(instance.solvable());

    const hd_public branch{ VECTOR1_M0H_1 };
    BOOST_REQUIRE(branch);
    const auto derived = branch.derive_public(7);
    const auto expected = chain::script::to_pay_key_hash_pattern(bitcoin_short_hash(derived.point()));

    const auto scripts = instance.scripts(7);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE(scripts.front().ops() == chain::operations(expected));
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__hardened_wildcard_public__empty)
{
    const descriptor instance("pkh(" VECTOR1_M0H "/1/*h)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.ranged());
    BOOST_REQUIRE(instance.scripts(0).empty());
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__origin_prefix__parsed)
{
    const descriptor instance("pkh([d34db33f/44h/0h]" VECTOR1_M0H "/1/*)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.ranged());
    BOOST_REQUIRE_EQUAL(instance.scripts(0).size(), 1u);
}

// signings

BOOST_AUTO_TEST_CASE(descriptor__signings__origin_pkh__origin_derivation)
{
    const descriptor instance("pkh([deadbeef/1h/2]" GENESIS_KEY ")");
    BOOST_REQUIRE(instance);

    const auto signings = instance.signings(0);
    BOOST_REQUIRE_EQUAL(signings.size(), 1u);
    BOOST_REQUIRE(signings.front().script == instance.scripts(0).front());
    BOOST_REQUIRE(!signings.front().embedded);
    BOOST_REQUIRE(!signings.front().witness);

    // The origin provides the fingerprint (little-endian) and leading path.
    const auto& derived = signings.front().derivations;
    BOOST_REQUIRE_EQUAL(derived.size(), 1u);
    BOOST_REQUIRE_EQUAL(derived.front().origin.fingerprint, 0xefbeadde_u32);

    const std_vector<uint32_t> path{ 0x80000001_u32, 2_u32 };
    BOOST_REQUIRE(derived.front().origin.path == path);
}

BOOST_AUTO_TEST_CASE(descriptor__signings__bare_key__own_fingerprint)
{
    const descriptor instance("pk(" GENESIS_KEY ")");
    BOOST_REQUIRE(instance);

    const auto signings = instance.signings(0);
    BOOST_REQUIRE_EQUAL(signings.size(), 1u);

    const auto& derived = signings.front().derivations;
    BOOST_REQUIRE_EQUAL(derived.size(), 1u);
    BOOST_REQUIRE(derived.front().origin.path.empty());

    const auto hash = bitcoin_short_hash(derived.front().point);
    const auto fingerprint = from_little_endian<uint32_t>(hash);
    BOOST_REQUIRE_EQUAL(derived.front().origin.fingerprint, fingerprint);
}

BOOST_AUTO_TEST_CASE(descriptor__signings__ranged_xpub__own_origin_with_index)
{
    const descriptor instance("pkh(" VECTOR1_M0H "/1/*)");
    BOOST_REQUIRE(instance);

    const auto signings = instance.signings(7);
    BOOST_REQUIRE_EQUAL(signings.size(), 1u);

    // An extended key without an origin prefix is its own origin.
    const hd_public parent{ VECTOR1_M0H };
    const auto& origin = signings.front().derivations.front().origin;
    const auto hash = bitcoin_short_hash(parent.point());
    const auto fingerprint = from_little_endian<uint32_t>(hash);
    BOOST_REQUIRE_EQUAL(origin.fingerprint, fingerprint);

    const std_vector<uint32_t> path{ 1_u32, 7_u32 };
    BOOST_REQUIRE(origin.path == path);
}

BOOST_AUTO_TEST_CASE(descriptor__signings__sh_wsh_multi__embedded_and_witness)
{
    const descriptor instance("sh(wsh(multi(1," GENESIS_KEY ")))");
    BOOST_REQUIRE(instance);

    const auto signings = instance.signings(0);
    BOOST_REQUIRE_EQUAL(signings.size(), 1u);

    // The p2sh embedded script is the p2wsh wrapper of the witness script.
    const auto& item = signings.front();
    BOOST_REQUIRE(item.embedded);
    BOOST_REQUIRE(item.witness);
    BOOST_REQUIRE(chain::script::is_pay_multisig_pattern(item.witness->ops()));
    BOOST_REQUIRE_EQUAL(item.derivations.size(), 1u);

    const auto witness_hash = sha256_hash(item.witness->to_data(false));
    const auto wrap = chain::script::to_pay_witness_script_hash_pattern(witness_hash);
    const chain::script wrapper{ wrap };
    BOOST_REQUIRE(*item.embedded == wrapper);

    const auto embedded_hash = bitcoin_short_hash(wrapper.to_data(false));
    const chain::script pay{ chain::script::to_pay_script_hash_pattern(embedded_hash) };
    BOOST_REQUIRE(item.script == pay);
}

// multi/sh/wsh

BOOST_AUTO_TEST_CASE(descriptor__scripts__sh_multi__expected_pattern)
{
    const descriptor instance("sh(multi(1," GENESIS_KEY "," GENESIS_KEY "))");
    BOOST_REQUIRE(instance);
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE(chain::script::is_pay_script_hash_pattern(scripts.front().ops()));
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__wsh_sortedmulti__expected_pattern)
{
    const descriptor instance("wsh(sortedmulti(1," VECTOR1_M0H "/1/0," VECTOR1_M0H "/1/1))");
    BOOST_REQUIRE(instance);
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE(chain::script::is_pay_witness_script_hash_pattern(scripts.front().ops()));
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__multi_zero_required__invalid)
{
    BOOST_REQUIRE(!descriptor("multi(0," GENESIS_KEY ")"));
}

// combo

BOOST_AUTO_TEST_CASE(descriptor__scripts__combo_uncompressed__two_scripts)
{
    const descriptor instance("combo(" GENESIS_KEY ")");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.scripts(0).size(), 2u);
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__combo_compressed__four_scripts)
{
    const descriptor instance("combo(" VECTOR1_M0H "/1/0)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.scripts(0).size(), 4u);
}

// tr key path (bip386 vectors)

BOOST_AUTO_TEST_CASE(descriptor__scripts__tr_xonly__expected_script)
{
    const descriptor instance("tr(a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.solvable());
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE_EQUAL(encode_base16(scripts.front().to_data(false)), "512077aab6e066f8a7419c5ab714c12c67d25007ed55a43cadcacb4d7a970a093f11");
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__tr_wif__expected_script)
{
    const descriptor instance("tr(L4rK1yDtCWekvXuE6oXD9jCYfFNV2cWRpVuPLBcCU2z8TrisoyY1)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.has_private_keys());
    const auto scripts = instance.scripts(0);
    BOOST_REQUIRE_EQUAL(scripts.size(), 1u);
    BOOST_REQUIRE_EQUAL(encode_base16(scripts.front().to_data(false)), "512077aab6e066f8a7419c5ab714c12c67d25007ed55a43cadcacb4d7a970a093f11");
}

BOOST_AUTO_TEST_CASE(descriptor__scripts__pkh_xonly__empty)
{
    const descriptor instance("pkh(a34b99f22c790c4e36b2b3c2c35a36db06226e41c692fc82b8b56ac1c540c5bd)");
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.scripts(0).empty());
}

BOOST_AUTO_TEST_SUITE_END()
