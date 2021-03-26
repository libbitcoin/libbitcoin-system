/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

#include <boost/test/unit_test.hpp>
#include <bitcoin/system.hpp>

using namespace bc::system;
using namespace bc::system::wallet;

BOOST_AUTO_TEST_SUITE(electrum_tests)

// These test cases were generated using electrum code and verified to
// match output from:
// github.com/spesmilo/electrum/blob/master/lib/mnemonic.py

#ifdef WITH_ICU

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__empty__valid)
{
    static const word_list mnemonic{ "" };
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "2e3749b0a2e6fbbd771b8f0e9227469d0b63d03eeb8c5cddd2a47d7c6f80d0275623ecd50952bef13c0216b52c1272d5bb6e3463e26a696c3d116ae2c4e980f3");
}

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__single__valid)
{
    static const word_list mnemonic{ "冬" };
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "6d0e28a68510d84a46db3daffdef720cb1f796cb68f66b2b5c3294a2805772ebbaa7665a3594addb4440c9024c25b75c80fadb89f1e9ac123dadc65e35e101af");
}

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__no_passphrase_1__valid)
{
    static const word_list mnemonic{ "foo", "bar", "baz" };
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "c4033901dd61ba26cfd0a1cf1ceb4b347606635aa3cb951eb6e819d58beedc04dd400a2e600d783c83c75879d6538abeecc7bb1b292b2a4d775d348d5d686427");
}

BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__no_passphrase_2__valid)
{
    static const word_list mnemonic
    {
        "giggle", "crush", "argue", "inflict", "wear", "defy", "combine", "evolve", "tiger", "spatial", "crumble", "fury"
    };
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "eb095e70f0eb8d24f2cef276c75b136c2ff41eb72a9291eb22ba45d11fbb875a965bd6ae09873543eea98bf3ae05067203cd56388e0978aebc3a511932704d80");
}

// This test conforms to an electrum test.
BOOST_AUTO_TEST_CASE(electrum__decode_mnemonic__passphrase__valid)
{
    static const word_list mnemonic{ "foobar" };
    const auto passphrase = "none";
    const auto seed = electrum::decode_mnemonic(mnemonic, passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "741b72fd15effece6bfe5a26a52184f66811bd2be363190e07a42cca442b1a5bb22b3ad0eb338197287e6d314866c7fba863ac65d3f156087a5052ebc7157fce");
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__no_dictionary__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "05e669b4270f4e25bce6fc3736170d423c");
    const auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "giggle crush argue inflict wear defy combine evolve tiger spatial crumble fury");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__en_dictionary__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "05e669b4270f4e25bce6fc3736170d423c");
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::en);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "giggle crush argue inflict wear defy combine evolve tiger spatial crumble fury");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::en));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__en_dictionary_prefix__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "0b0d80f992a51348a89aeb5196fa0bd0d6");
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::en, electrum::seed::standard);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "crawl consider laptop bonus stove chase earn battle feed town scatter radio");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::en, electrum::seed::standard));
}

// This test conforms to an electrum test.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__zh_Hans_dictionary__witness_prefix__valid)
{
    const std::string seed_words = "眼 悲 叛 改 节 跃 衡 响 疆 股 遂 冬";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(seed_words)), "e79cbc20e682b220e58f9b20e694b920e88a8220e8b78320e8a1a120e5938d20e7968620e882a120e9818220e586ac");
    const std::string passphrase = "给我一些测试向量谷歌";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(passphrase)), "e7bb99e68891e4b880e4ba9be6b58be8af95e59091e9878fe8b0b7e6ad8c");

    data_chunk entropy;
    decode_base16(entropy, "090ff228d676340e9ad295e25d9fef11cb"); // 3083737086352778425940060465574397809099
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::zh_Hans, electrum::seed::witness);
    BOOST_REQUIRE(!mnemonic.empty());

    // Test for seed without passphrase
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "0b9077db7b5a50dbb6f61821e2d35e255068a5847e221138048a20e12d80b673ce306b6fe7ac174ebc6751e11b7037be6ee9f17db8040bb44f8466d519ce2abf");

    // Test for seed with passphrase
    const auto seed_with_passphrase = electrum::decode_mnemonic(mnemonic, passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(seed_with_passphrase), "6c03dd0615cf59963620c0af6840b52e867468cc64f20a1f4c8155705738e87b8edb0fc8a6cee4085776cb3a629ff88bb1a38f37085efdbf11ce9ec5a7fa5f71");
}

// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__ja_dictionary_prefix__valid)
{
    // Note: electrum seed words for this example are already normalized to
    // nfkd, but we normally rely on the process to normalize them.  This
    // test however requires pre-normalization to work.
    const std::string seed_words = "なのか ひろい しなん まなぶ つぶす さがす おしゃれ かわく おいかける けさき かいとう さたん";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(to_normal_nfkd_form(seed_words))), "e381aae381aee3818b20e381b2e3828de3818420e38197e381aae3829320e381bee381aae381b5e3829920e381a4e381b5e38299e3819920e38195e3818be38299e3819920e3818ae38197e38283e3828c20e3818be3828fe3818f20e3818ae38184e3818be38191e3828b20e38191e38195e3818d20e3818be38184e381a8e3818620e38195e3819fe38293");

    // Japanese language mnemonic test with no passphrase.
    data_chunk entropy;
    decode_base16(entropy, "05b251d0b0f32da46966cd6e16ca740d6d"); // 1938439226660562861250521787963972783469
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::ja, electrum::seed::standard);
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::ja, electrum::seed::standard));
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "d3eaf0e44ddae3a5769cb08a26918e8b308258bcb057bb704c6f69713245c0b35cb92c03df9c9ece5eff826091b4e74041e010b701d44d610976ce8bfb66a8ad");

    // Sanity check that the mnemonic we generated from the entropy
    // matches the original seed_words provided by the electrum example.
    const auto seed_from_seed_words = electrum::decode_mnemonic(split(seed_words));
    BOOST_REQUIRE_EQUAL(encode_base16(seed_from_seed_words), encode_base16(seed));

    // Unicode horror testing.
    const std::string unicode_horror_hex = "e282bf20f09f988020f09f98882020202020e3818620e38191e3819fe381be20e3828fe3828b2077cda2cda2cd9d68cda16fcda2cda120ccb8cda26bccb5cd9f6eccb4cd98c7ab77ccb8cc9b73cd9820cc80cc8177cd98cda2e1b8a9ccb561d289cca1cda27420cca7cc9568cc816fccb572cd8fccb5726f7273cca120ccb6cda1cda06cc4afccb665cd9fcd9f20ccb6cd9d696ecda220cd8f74cc9568ccb7cca1cd9f6520cd9fcd9f64cc9b61cd9c72cc95cda16bcca2cca820cda168ccb465cd8f61ccb7cca2cca17274cc81cd8f20ccb4ccb7cda0c3b2ccb5ccb666ccb82075cca7cd986ec3adcc9bcd9c63cda2cd8f6fccb7cd8f64ccb8cda265cca1cd9d3fcd9e";

    data_chunk unicode_horror;
    decode_base16(unicode_horror, unicode_horror_hex);
    BOOST_REQUIRE_EQUAL(encode_base16(unicode_horror), unicode_horror_hex);

    // Japanese mnemonic test using unicode_horror as passphrase.
    const auto pass = std::string(unicode_horror.begin(), unicode_horror.end());
    const auto seed_with_passphrase = electrum::decode_mnemonic(mnemonic, pass);
    BOOST_REQUIRE_EQUAL(encode_base16(seed_with_passphrase), "251ee6b45b38ba0849e8f40794540f7e2c6d9d604c31d68d3ac50c034f8b64e4bc037c5e1e985a2fed8aad23560e690b03b120daf2e84dceb1d7857dda042457");
}

// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__en_dictionary_prefix_witness__valid)
{
    const auto mnemonic = split("wild father tree among universe such mobile favorite target dynamic credit identify");
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "aac2a6302e48577ab4b46f23dbae0774e2e62c796f797d0a1b5faeb528301e3064342dafb79069e7c4c6b8c38ae11d7a973bec0d4f70626f8cc5184a8d0b0756");
}


// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__en_dictionary_prefix_witness_with_passphrase__valid)
{
    const auto mnemonic = split("wild father tree among universe such mobile favorite target dynamic credit identify");
    const auto passphrase = "Did you ever hear the tragedy of Darth Plagueis the Wise?";
    const auto seed = electrum::decode_mnemonic(mnemonic, passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "4aa29f2aeb0127efb55138ab9e7be83b36750358751906f86c662b21a1ea1370f949e6d1a12fa56d3d93cadda93038c76ac8118597364e46f5156fde6183c82f");
}

// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__es_dictionary_prefix__valid)
{
    const std::string seed_words = "almíbar tibio superar vencer hacha peatón príncipe matar consejo polen vehículo odisea";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(to_normal_nfkd_form(seed_words))), "616c6d69cc8162617220746962696f20737570657261722076656e63657220686163686120706561746fcc816e20707269cc816e63697065206d6174617220636f6e73656a6f20706f6c656e2076656869cc8163756c6f206f6469736561");

    data_chunk entropy;
    decode_base16(entropy, "0a0fecede9bf8a975eb6b4ef75bb79a04f"); // 3423992296655289706780599506247192518735
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::es);
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::es));
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "18bffd573a960cc775bbd80ed60b7dc00bc8796a186edebe7fc7cf1f316da0fe937852a969c5c79ded8255cdf54409537a16339fbe33fb9161af793ea47faa7a");
}

// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__es_dictionary_prefix_with_passphrase__valid)
{
    const std::string seed_words = "almíbar tibio superar vencer hacha peatón príncipe matar consejo polen vehículo odisea";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(to_normal_nfkd_form(seed_words))), "616c6d69cc8162617220746962696f20737570657261722076656e63657220686163686120706561746fcc816e20707269cc816e63697065206d6174617220636f6e73656a6f20706f6c656e2076656869cc8163756c6f206f6469736561");

    const std::string passphrase = "araña difícil solución término cárcel";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(to_normal_nfkd_form(passphrase))), "6172616ecc83612064696669cc8163696c20736f6c7563696fcc816e207465cc81726d696e6f206361cc817263656c");

    data_chunk entropy;
    decode_base16(entropy, "0a0fecede9bf8a975eb6b4ef75bb79a04f"); // 3423992296655289706780599506247192518735
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::es);
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::es));
    const auto seed = electrum::decode_mnemonic(mnemonic, passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "363dec0e575b887cfccebee4c84fca5a3a6bed9d0e099c061fa6b85020b031f8fe3636d9af187bf432d451273c625e20f24f651ada41aae2c4ea62d87e9fa44c");
}

// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__es_dictionary_prefix_witness__valid)
{
    const std::string seed_words = "equipo fiar auge langosta hacha calor trance cubrir carro pulmón oro áspero";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(to_normal_nfkd_form(seed_words))), "65717569706f20666961722061756765206c616e676f7374612068616368612063616c6f72207472616e63652063756272697220636172726f2070756c6d6fcc816e206f726f2061cc81737065726f");

    data_chunk entropy;
    decode_base16(entropy, "01514c6f795e3bfdaca034e7ac2ed60275"); // 448346710104003081119421156750490206837
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::es, electrum::seed::witness);
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::es, electrum::seed::witness));
    const auto seed = electrum::decode_mnemonic(mnemonic);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "001ebce6bfde5851f28a0d44aae5ae0c762b600daf3b33fc8fc630aee0d207646b6f98b18e17dfe3be0a5efe2753c7cdad95860adbbb62cecad4dedb88e02a64");
}

// This test conforms to one or more electrum tests.
BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__es_dictionary_prefix_witness_with_passphrase__valid)
{
    const std::string seed_words = "vidrio jabón muestra pájaro capucha eludir feliz rotar fogata pez rezar oír";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(to_normal_nfkd_form(seed_words))), "76696472696f206a61626fcc816e206d756573747261207061cc816a61726f206361707563686120656c756469722066656c697a20726f74617220666f676174612070657a2072657a6172206f69cc8172");

    // This passphrase is not pre-normalized.
    const std::string passphrase = "¡Viva España! repiten veinte pueblos y al hablar dan fe del ánimo español... ¡Marquen arado martillo y clarín";
    BOOST_REQUIRE_EQUAL(encode_base16(to_chunk(passphrase)), "c2a1566976612045737061c3b16121207265706974656e207665696e746520707565626c6f73207920616c206861626c61722064616e2066652064656c20c3a16e696d6f2065737061c3b16f6c2e2e2e20c2a14d61727175656e20617261646f206d617274696c6c6f207920636c6172c3ad6e");

    data_chunk entropy;
    decode_base16(entropy, "0a1f92ec9adfcdcadd2c955a8f2a5d47d0"); // 3444792611339130545499611089352232093648
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::es, electrum::seed::witness);
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::es, electrum::seed::witness));
    const auto seed = electrum::decode_mnemonic(mnemonic, passphrase);
    BOOST_REQUIRE_EQUAL(encode_base16(seed), "c274665e5453c72f82b8444e293e048d700c59bf000cacfba597629d202dcf3aab1cf9c00ba8d3456b7943428541fed714d01d8a0a4028fc3a9bb33d981cb49f");
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__es_dictionary_prefix_standard__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "05e669b4270f4e25bce6fc3736170d423c");
    const auto mnemonic = electrum::create_mnemonic(entropy, language::electrum::es, electrum::seed::standard);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "gráfico codo ámbar insecto verbo cráter celoso entrar tarjeta sala coco frito");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::es, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__large_entropy_1__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "2b18cf24d3c26529b18d733abadff49f06e65ebf74263f0ba00d11a9b0cba8d303");
    const auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "icon person grape only cash addict fringe disease luggage worry consider hover dignity wood street deny purpose shiver network chaos pole since shoe climb");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::en, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__large_entropy_2__valid)
{
    data_chunk entropy;
    decode_base16(entropy, "b0756302179e800b182514c729f1d6814c377ff06097569ef540e6c1f1950f08");
    const auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    BOOST_REQUIRE_EQUAL(join(mnemonic), "divide february web hire limb run reject nuclear army zone brick below public ladder deer below again cluster divorce ketchup aerobic flee lonely absent");
    BOOST_REQUIRE(electrum::validate_mnemonic(mnemonic, entropy, language::electrum::en, electrum::seed::standard));
}

BOOST_AUTO_TEST_CASE(electrum__create_mnemonic__incorrent_mnemonic__invalid)
{
    data_chunk entropy;
    decode_base16(entropy, "2b18cf24d3c26529b18d733abadff49f06e65ebf74263f0ba00d11a9b0cba8d303");
    auto mnemonic = electrum::create_mnemonic(entropy);
    BOOST_REQUIRE(!mnemonic.empty());
    mnemonic[0] = "invalid";
    BOOST_REQUIRE(!electrum::validate_mnemonic(mnemonic, entropy, language::electrum::en, electrum::seed::standard));
}

#endif

BOOST_AUTO_TEST_SUITE_END()
