/////**
//// * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
//// *
//// * This file is part of libbitcoin.
//// *
//// * This program is free software: you can redistribute it and/or modify
//// * it under the terms of the GNU Affero General Public License as published by
//// * the Free Software Foundation, either version 3 of the License, or
//// * (at your option) any later version.
//// *
//// * This program is distributed in the hope that it will be useful,
//// * but WITHOUT ANY WARRANTY; without even the implied warranty of
//// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// * GNU Affero General Public License for more details.
//// *
//// * You should have received a copy of the GNU Affero General Public License
//// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//// */
////#include <boost/test/unit_test.hpp>
////#include <bitcoin/system.hpp>
////#include "mnemonic.hpp"
////
////using namespace bc::system;
////using namespace bc::system::wallet;
////
////BOOST_AUTO_TEST_SUITE(mnemonic_tests)
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__trezor_vectors__success)
////{
////    for (const auto& vector: mnemonic_trezor_en_vectors)
////    {
////        data_chunk entropy;
////        BOOST_REQUIRE(decode_base16(entropy, vector.entropy));
////        mnemonic instance(entropy);
////        BOOST_REQUIRE(instance);
////        BOOST_REQUIRE(instance.lexicon() == reference::en);
////        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
////        BOOST_REQUIRE_EQUAL(instance.words(), split(vector.mnemonic));
////        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);
////
////#ifdef WITH_ICU
////        const auto seed = instance.to_seed(vector.passphrase);
////        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
////        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
////#endif
////    }
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__japanese_vectors__success)
////{
////    for (const auto& vector: mnemonic_bip39jp_jp_vectors)
////    {
////        data_chunk entropy;
////        BOOST_REQUIRE(decode_base16(entropy, vector.entropy));
////        mnemonic instance(entropy, reference::ja);
////        BOOST_REQUIRE(instance);
////        BOOST_REQUIRE(instance.lexicon() == reference::ja);
////        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
////        BOOST_REQUIRE_EQUAL(instance.words(), split_regex(vector.mnemonic, mnemonic::ideographic_space));
////        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);
////
////#ifdef WITH_ICU
////        const auto seed = instance.to_seed(vector.passphrase);
////        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
////        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
////#endif
////    }
////}
////
////// construct2
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__trezor_vectors__success)
////{
////    for (const auto& vector: mnemonic_trezor_en_vectors)
////    {
////        mnemonic instance(vector.mnemonic);
////        BOOST_REQUIRE(instance);
////        BOOST_REQUIRE(instance.lexicon() == reference::en);
////        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
////        BOOST_REQUIRE_EQUAL(instance.words(), split(vector.mnemonic));
////        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);
////
////#ifdef WITH_ICU
////        const auto seed = instance.to_seed(vector.passphrase);
////        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
////        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
////#endif
////    }
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__japanese_vectors__success)
////{
////    for (const auto& vector: mnemonic_bip39jp_jp_vectors)
////    {
////        mnemonic instance(vector.mnemonic, reference::ja);
////        BOOST_REQUIRE(instance);
////        BOOST_REQUIRE(instance.lexicon() == reference::ja);
////        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
////        BOOST_REQUIRE_EQUAL(instance.words(), split_regex(vector.mnemonic, mnemonic::ideographic_space));
////        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);
////
////#ifdef WITH_ICU
////        const auto seed = instance.to_seed(vector.passphrase);
////        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
////        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
////#endif
////    }
////}
////
////// construct (pathological) 
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__misspelled_en__false)
////{
////    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon aboot", reference::en));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__length_one_en__false)
////{
////    BOOST_REQUIRE(!mnemonic("one", reference::en));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__length_two_en__false)
////{
////    BOOST_REQUIRE(!mnemonic("one two", reference::en));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__length_eleven_en__false)
////{
////    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon", reference::en));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__bad_checksum_en__false)
////{
////    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon one", reference::en));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__incorrect_language__false)
////{
////    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon agent", reference::zh_Hant));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__correct_language__true)
////{
////    BOOST_REQUIRE(mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon agent", reference::en));
////}
////
////BOOST_AUTO_TEST_CASE(mnemonic__construct1__any_language__true)
////{
////    BOOST_REQUIRE(mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon agent"));
////}
////
////BOOST_AUTO_TEST_SUITE_END()
