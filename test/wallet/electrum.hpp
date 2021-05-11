/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_ELECTRUM_HPP
#define LIBBITCOIN_SYSTEM_TEST_ELECTRUM_HPP

#include <string>
#include <bitcoin/system.hpp>

// Avoid using namespace in shared headers, but okay here.
using namespace bc::system;
using namespace bc::system::wallet;
using prefix = electrum::seed_prefix;

// All mnemonics here are 12 words and entropy is 20 bytes.
// Japanese mnemonics are ascii space delimited (not ideographic space).
class electrum_result
{
public:
    std::string name;
    language lingo;
    std::string mnemonic;
    data_chunk mnemonic_chunk;
    prefix prefix;
    data_chunk entropy;
    std::string passphrase;
    data_chunk passphrase_chunk;
    long_hash hd_seed;

    hd_private to_hd() const
    {
        // Split hd seed into secret and chaincode.
        const auto part = system::split(hd_seed);
        return { part.first, part.second, hd_private::mainnet };
    }
};

typedef std::vector<electrum_result> electrum_result_list;

// github.com/spesmilo/electrum/blob/master/electrum/tests/test_mnemonic.py

electrum_result_list vectors
{
    electrum_result
    {
        "english",
        language::en,
        "wild father tree among universe such mobile favorite target dynamic credit identify",
        {},
        prefix::witness,
        {},
        "",
        {},
        base16_array("aac2a6302e48577ab4b46f23dbae0774e2e62c796f797d0a1b5faeb528301e3064342dafb79069e7c4c6b8c38ae11d7a973bec0d4f70626f8cc5184a8d0b0756")
    },
    electrum_result
    {
        "english_with_passphrase",
        language::en,
        "wild father tree among universe such mobile favorite target dynamic credit identify",
        {},
        prefix::witness,
        {},
        "Did you ever hear the tragedy of Darth Plagueis the Wise?",
        {},
        base16_array("4aa29f2aeb0127efb55138ab9e7be83b36750358751906f86c662b21a1ea1370f949e6d1a12fa56d3d93cadda93038c76ac8118597364e46f5156fde6183c82f")
    },
    electrum_result
    {
        "japanese",
        language::ja,
        "なのか ひろい しなん まなぶ つぶす さがす おしゃれ かわく おいかける けさき かいとう さたん",
        base16_chunk("e381aae381aee3818b20e381b2e3828de3818420e38197e381aae3829320e381bee381aae381b5e3829920e381a4e381b5e38299e3819920e38195e3818be38299e3819920e3818ae38197e38283e3828c20e3818be3828fe3818f20e3818ae38184e3818be38191e3828b20e38191e38195e3818d20e3818be38184e381a8e3818620e38195e3819fe38293"),
        prefix::standard,
        base16_chunk("1938439226660562861250521787963972783469"),
        "",
        {},
        base16_array("d3eaf0e44ddae3a5769cb08a26918e8b308258bcb057bb704c6f69713245c0b35cb92c03df9c9ece5eff826091b4e74041e010b701d44d610976ce8bfb66a8ad")
    },
    electrum_result
    {
        "japanese_with_passphrase",
        language::ja,
        "なのか ひろい しなん まなぶ つぶす さがす おしゃれ かわく おいかける けさき かいとう さたん",
        base16_chunk("e381aae381aee3818b20e381b2e3828de3818420e38197e381aae3829320e381bee381aae381b5e3829920e381a4e381b5e38299e3819920e38195e3818be38299e3819920e3818ae38197e38283e3828c20e3818be3828fe3818f20e3818ae38184e3818be38191e3828b20e38191e38195e3818d20e3818be38184e381a8e3818620e38195e3819fe38293"),
        prefix::standard,
        base16_chunk("1938439226660562861250521787963972783469"),
        "'₿ 😀 😈     う けたま わる w͢͢͝h͡o͢͡ ̸͢k̵͟n̴͘ǫw̸̛s͘ ̀́w͘͢ḩ̵a҉̡͢t ̧̕h́o̵r͏̵rors̡ ̶͡͠lį̶e͟͟ ̶͝in͢ ͏t̕h̷̡͟e ͟͟d̛a͜r̕͡k̢̨ ͡h̴e͏a̷̢̡rt́͏ ̴̷͠ò̵̶f̸ u̧͘ní̛͜c͢͏o̷͏d̸͢e̡͝?͞'",
        base16_chunk("e282bf20f09f988020f09f98882020202020e3818620e38191e3819fe381be20e3828fe3828b2077cda2cda2cd9d68cda16fcda2cda120ccb8cda26bccb5cd9f6eccb4cd98c7ab77ccb8cc9b73cd9820cc80cc8177cd98cda2e1b8a9ccb561d289cca1cda27420cca7cc9568cc816fccb572cd8fccb5726f7273cca120ccb6cda1cda06cc4afccb665cd9fcd9f20ccb6cd9d696ecda220cd8f74cc9568ccb7cca1cd9f6520cd9fcd9f64cc9b61cd9c72cc95cda16bcca2cca820cda168ccb465cd8f61ccb7cca2cca17274cc81cd8f20ccb4ccb7cda0c3b2ccb5ccb666ccb82075cca7cd986ec3adcc9bcd9c63cda2cd8f6fccb7cd8f64ccb8cda265cca1cd9d3fcd9e"),
        base16_array("251ee6b45b38ba0849e8f40794540f7e2c6d9d604c31d68d3ac50c034f8b64e4bc037c5e1e985a2fed8aad23560e690b03b120daf2e84dceb1d7857dda042457")
    },
    electrum_result
    {
        "chinese",
        language::zh_Hans,
        "眼 悲 叛 改 节 跃 衡 响 疆 股 遂 冬",
        base16_chunk("e79cbc20e682b220e58f9b20e694b920e88a8220e8b78320e8a1a120e5938d20e7968620e882a120e9818220e586ac"),
        prefix::witness,
        base16_chunk("3083737086352778425940060465574397809099"),
        "",
        {},
        base16_array("0b9077db7b5a50dbb6f61821e2d35e255068a5847e221138048a20e12d80b673ce306b6fe7ac174ebc6751e11b7037be6ee9f17db8040bb44f8466d519ce2abf")
    },
    electrum_result
    {
        "chinese_with_passphrase",
        language::zh_Hans,
        "眼 悲 叛 改 节 跃 衡 响 疆 股 遂 冬",
        base16_chunk("e79cbc20e682b220e58f9b20e694b920e88a8220e8b78320e8a1a120e5938d20e7968620e882a120e9818220e586ac"),
        prefix::witness,
        base16_chunk("3083737086352778425940060465574397809099"),
        "给我一些测试向量谷歌",
        base16_chunk("e7bb99e68891e4b880e4ba9be6b58be8af95e59091e9878fe8b0b7e6ad8c"),
        base16_array("6c03dd0615cf59963620c0af6840b52e867468cc64f20a1f4c8155705738e87b8edb0fc8a6cee4085776cb3a629ff88bb1a38f37085efdbf11ce9ec5a7fa5f71")
    },
    electrum_result
    {
        "spanish",
        language::es,
        "almíbar tibio superar vencer hacha peatón príncipe matar consejo polen vehículo odisea",
        base16_chunk("616c6d69cc8162617220746962696f20737570657261722076656e63657220686163686120706561746fcc816e20707269cc816e63697065206d6174617220636f6e73656a6f20706f6c656e2076656869cc8163756c6f206f6469736561"),
        prefix::standard,
        base16_chunk("3423992296655289706780599506247192518735"),
        "",
        {},
        base16_array("18bffd573a960cc775bbd80ed60b7dc00bc8796a186edebe7fc7cf1f316da0fe937852a969c5c79ded8255cdf54409537a16339fbe33fb9161af793ea47faa7a")
    },
    electrum_result
    {
        "spanish_with_passphrase",
        language::es,
        "almíbar tibio superar vencer hacha peatón príncipe matar consejo polen vehículo odisea",
        base16_chunk("616c6d69cc8162617220746962696f20737570657261722076656e63657220686163686120706561746fcc816e20707269cc816e63697065206d6174617220636f6e73656a6f20706f6c656e2076656869cc8163756c6f206f6469736561"),
        prefix::standard,
        base16_chunk("3423992296655289706780599506247192518735"),
        "araña difícil solución término cárcel",
        base16_chunk("6172616ecc83612064696669cc8163696c20736f6c7563696fcc816e207465cc81726d696e6f206361cc817263656c"),
        base16_array("363dec0e575b887cfccebee4c84fca5a3a6bed9d0e099c061fa6b85020b031f8fe3636d9af187bf432d451273c625e20f24f651ada41aae2c4ea62d87e9fa44c")
    },
    electrum_result
    {
        "spanish2",
        language::es,
        "equipo fiar auge langosta hacha calor trance cubrir carro pulmón oro áspero",
        base16_chunk("65717569706f20666961722061756765206c616e676f7374612068616368612063616c6f72207472616e63652063756272697220636172726f2070756c6d6fcc816e206f726f2061cc81737065726f"),
        prefix::witness,
        base16_chunk("0448346710104003081119421156750490206837"), // one char short in original, padded left.
        "",
        {},
        base16_array("001ebce6bfde5851f28a0d44aae5ae0c762b600daf3b33fc8fc630aee0d207646b6f98b18e17dfe3be0a5efe2753c7cdad95860adbbb62cecad4dedb88e02a64")
    },
    electrum_result
    {
        "spanish3",
        language::es,
        "vidrio jabón muestra pájaro capucha eludir feliz rotar fogata pez rezar oír",
        base16_chunk("76696472696f206a61626fcc816e206d756573747261207061cc816a61726f206361707563686120656c756469722066656c697a20726f74617220666f676174612070657a2072657a6172206f69cc8172"),
        prefix::witness,
        base16_chunk("3444792611339130545499611089352232093648"),
        "¡Viva España! repiten veinte pueblos y al hablar dan fe del ánimo español... ¡Marquen arado martillo y clarín",
        base16_chunk("c2a1566976612045737061c3b16121207265706974656e207665696e746520707565626c6f73207920616c206861626c61722064616e2066652064656c20c3a16e696d6f2065737061c3b16f6c2e2e2e20c2a14d61727175656e20617261646f206d617274696c6c6f207920636c6172c3ad6e"),
        base16_array("c274665e5453c72f82b8444e293e048d700c59bf000cacfba597629d202dcf3aab1cf9c00ba8d3456b7943428541fed714d01d8a0a4028fc3a9bb33d981cb49f")
    }
};

#endif