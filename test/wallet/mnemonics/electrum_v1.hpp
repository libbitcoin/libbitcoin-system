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
#ifndef LIBBITCOIN_SYSTEM_TEST_ELECTRUM_V1_HPP
#define LIBBITCOIN_SYSTEM_TEST_ELECTRUM_V1_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <bitcoin/system.hpp>

 // Avoid using namespace in shared headers, but okay here.
using namespace bc::system;
using namespace bc::system::wallet;

namespace test {
namespace mnemonics_electrum_v1 {

struct electrum_v1_vector
{
    language lingo;
    std::string mnemonic;
    data_chunk entropy;
    std::string key;
    context network;
};

typedef std::vector<electrum_v1_vector> electrum_v1_vectors;

// Electrum v1 mnemonic validation algorithm (from Electrum).
// github.com/spesmilo/electrum/blob/master/electrum/old_mnemonic.py
// This was used to generate the test vectors below.
//
//words = [
//"like",
//"just",
//  ...
//"weapon",
//"weary"
//]
// 
//n = 1626
//
//def mn_encode( message ):
//    out = []
//    for i in range(len(message)/8):
//        word = message[8*i:8*i+8]
//        x = int(word, 16)
//        w1 = (x%n)
//        w2 = ((x/n) + w1)%n
//        w3 = ((x/n/n) + w2)%n
//        out += [ words[w1], words[w2], words[w3] ]
//    return out
//
//def mn_decode( wlist ):
//    out = ''
//    for i in range(len(wlist)/3):
//        word1, word2, word3 = wlist[3*i:3*i+3]
//        w1 =  words.index(word1)
//        w2 = (words.index(word2))%n
//        w3 = (words.index(word3))%n
//        x = w1 +n*((w2-w1)%n) +n*n*((w3-w2)%n)
//        out += '%08x'%x
//    return out
//
// print(mn_encode("c6e913bc397bef94c6e913bc397bef94"))
// print(mn_decode(['blind', 'faith', 'blind', 'faith', 'blind', 'faith', 'blind', 'faith', 'blind', 'faith', 'blind', 'faith']))
// ...
// ['blind', 'faith', 'blind', 'faith', 'blind', 'faith', 'blind', 'faith', 'blind', 'faith', 'blind', 'faith']
// c6e913bc397bef94c6e913bc397bef94

const auto mnemonic =
    "powerful random nobody notice nothing important "
    "anyway look away hidden message over";

const auto master_public_key = "04"
    "e9d4b7866dd1e91c862aebf62a49548c7dbf7bcc6e4b7b8c9da820c7737968df"
    "9c09d5a3e271dc814a29981f81b3faaf2737b551ef5dcc6189cf0f8252c442b3";

// Wrapper avoids static initialization race between vectors and hd_private.
electrum_v1_vectors vectors()
{

    static const electrum_v1_vectors delayed_initialize_vectors
    {
        electrum_v1_vector
        {
            // electrum/tests/test_wallet_vertical.py#L153
            // electrum/tests/test_wallet_vertical.py#L2952
            // mpk: e9d4b7866dd1e91c862aebf62a49548c7dbf7bcc6e4b7b8c9da820c7737968df9c09d5a3e271dc814a29981f81b3faaf2737b551ef5dcc6189cf0f8252c442b3
            language::en,
            "powerful random nobody notice nothing important anyway look away hidden message over",
            base16_chunk("e440b7ac4913c354f1c8d7011ccc9764"),
            "xprv9s21ZrQH143K2d2hyk2XNBAcwpHuLKp12yifaDbS3dT2Ed9FB969CBr2uUyuYnvu3nUiWv7cWXWC36rHxZ5JQjkACk8jMBDfupZ8cRhNeRS",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            // electrum/tests/test_wallet_vertical.py#L2210
            // mpk: cd805ed20aec61c7a8b409c121c6ba60a9221f46d20edbc2be83ebd91460e97937cd7d782e77c1cb08364c6bc1c98bc040fdad53f22f29f7d3a85c8e51f9c875
            language::en,
            "alone body father children lead goodbye phone twist exist grass kick join",
            base16_chunk("749a03149d2d16001fc3c30cdc8d1600"),
            // backfilled:
            "xprv9s21ZrQH143K3YSbAXLMPCzJso5QAarQksAGc5rQCyZCBfw4Rj2PqVLFNgezSBhktYkiL3Ta2stLPDF9yZtLMaxk6Spiqh3DNFG8p8MVeEC",
            btc_mainnet_p2kh
        },

        electrum_v1_vector
        {
            language::en,
            "like like like like like like like like like like like like",
            base16_chunk("00000000000000000000000000000000"),
            "xprv9s21ZrQH143K2JbpEjGU94NcdKSASB7LuXvJCTsxuENcGN1nVG7QjMnBZ6zZNcJaiJogsRaLaYFFjs48qt4Fg7y1GnmrchQt1zFNu6QVnta",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "like like like like like like like like like like like like like like like like like like like like like like like like",
            base16_chunk("0000000000000000000000000000000000000000000000000000000000000000"),
            "tprv8ZgxMBicQKsPe2MzCEmfUKuUyaerdtY5tR54SKCw3jKsmrjXixa4NFZTqjDvntbEnZPBL861gQ1FNQCDGaCBEpH7iZNWUHqexCgDRha6qRu",
            btc_testnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "fail husband howl fail husband howl fail husband howl fail husband howl",
            base16_chunk("ffffffffffffffffffffffffffffffff"),
            "xprv9s21ZrQH143K4QPkuYJSfBgCeAp1QAfdjrncnwaxixCaDVHtRXN54iV7zfpHTRWGgGCVgun35Jgq2BwoAboDrvyMU1YEmd4LPawcbJiHJ2G",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl",
            base16_chunk("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
            "tprv8ZgxMBicQKsPd832P7W6TgCXujftB87yVhuambjjoo77bVjG3E1StNnyGrmLuLtM3NDMt7rohJXC56iDHGiDmugobkBfpwqyXYbzUD6LQ9A",
            btc_testnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "foe foe foe foe foe foe foe foe foe foe foe foe",
            base16_chunk("30060000300600003006000030060000"),
            "xprv9s21ZrQH143K3ntSzz7MWLryYLSLJQcQKBFdEewciQJ4j3jTaTLH9i1ypqZCkgEPRo4j2uSp4NUAETaV9BKT5HCcnaC5beD8QsruYwAETh9",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe",
            base16_chunk("3006000030060000300600003006000030060000300600003006000030060000"),
            "tprv8ZgxMBicQKsPcvKG7ngskjC2xoLQ3QtRdFAmLv8U2grMWc5NGDbaPcZNCPMyXM5vHqSXc5tchthxW8DLkpFsH6bvpHRzrRhnhv8uXvAKePF",
            btc_testnet_p2kh
        },
        electrum_v1_vector
        {
            language::pt,
            "vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito",
            base16_chunk("30060000300600003006000030060000"),
            "xprv9s21ZrQH143K3ntSzz7MWLryYLSLJQcQKBFdEewciQJ4j3jTaTLH9i1ypqZCkgEPRo4j2uSp4NUAETaV9BKT5HCcnaC5beD8QsruYwAETh9",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            language::pt,
            "vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito",
            base16_chunk("3006000030060000300600003006000030060000300600003006000030060000"),
            "tprv8ZgxMBicQKsPcvKG7ngskjC2xoLQ3QtRdFAmLv8U2grMWc5NGDbaPcZNCPMyXM5vHqSXc5tchthxW8DLkpFsH6bvpHRzrRhnhv8uXvAKePF",
            btc_testnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "blind faith blind faith blind faith blind faith blind faith blind faith",
            base16_chunk("bc13e9c694ef7b39bc13e9c694ef7b39"),
            "xprv9s21ZrQH143K3hSVhNZzLwbs5wVLEePFhqMYDnE8gY1uN4teV9J7dzQv4BPx4mCnqBeuj9YDPYMnKs1gEJ7qRs5EycGxYBANzgFbJjEd1Bx",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            language::en,
            "blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith",
            base16_chunk("bc13e9c694ef7b39bc13e9c694ef7b39bc13e9c694ef7b39bc13e9c694ef7b39"),
            "tprv8ZgxMBicQKsPePuBrXpFdiwWBLB5kC486mKYg57DghzrSK4WKapEGFYo7e94XzjxQPq5jwM7p4nswPQyBgPEysBtwWp4sBqbUuzMEZPT1qB",
            btc_testnet_p2kh
        },
        electrum_v1_vector
        {
            language::pt,
            "gear ansioso medusa bicuspide lauto ciatico jesus doping iceberg empuxo harpista feixe",
            base16_chunk("5089bd85f14b53a3db987b362240fcea"),
            "xprv9s21ZrQH143K2fdDmJuESB5D5PmTSk5pvFUmt15LdJTxFXD6PB84GyQMfZZq3kmYPX9HGeNtp238i1UMWbLmvU7q2aQWf524kjF1E9a5MvF",
            btc_mainnet_p2kh
        },
        electrum_v1_vector
        {
            language::pt,
            "gear ansioso medusa bicuspide lauto ciatico jesus doping iceberg empuxo harpista feixe gear ansioso medusa bicuspide lauto ciatico jesus doping iceberg empuxo harpista feixe",
            base16_chunk("5089bd85f14b53a3db987b362240fcea5089bd85f14b53a3db987b362240fcea"),
            "tprv8ZgxMBicQKsPdzodqoReoGJPeBhMXcJ5DBp9yPZby5zC4axqffvzgLCcRcFGzcXCFJiUNN7K6k5LKSJhwFtDCFNtv2m1tByGusiU2wDc2iW",
            btc_testnet_p2kh
        }
    };

    return delayed_initialize_vectors;
}

// invalid for length
const string_list words2
{
    "blind", "faith"
};

// mixed valid languages
const string_list mixed_words12
{
    "abissinio", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

// valid
const string_list words12
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

// valid
const string_list words24
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

// invalid for length
const string_list words26
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith"
};

// Test wrapper for access to protected methods.
class accessor
  : public electrum_v1
{
public:
    accessor(const data_chunk& entropy, const string_list& words,
        language identifier)
      : electrum_v1(entropy, words, identifier)
    {
    }

    static size_t entropy_bits(const data_slice& entropy)
    {
        return electrum_v1::entropy_bits(entropy);
    }

    static size_t entropy_bits(const string_list& words)
    {
        return electrum_v1::entropy_bits(words);
    }

    static size_t entropy_size(const string_list& words)
    {
        return electrum_v1::entropy_bits(words);
    }

    static size_t word_count(const data_slice& entropy)
    {
        return electrum_v1::word_count(entropy);
    }

    static data_chunk decoder(const string_list& words, language identifier)
    {
        return electrum_v1::decoder(words, identifier);
    }

    static string_list encoder(const data_chunk& entropy, language identifier)
    {
        return electrum_v1::encoder(entropy, identifier);
    }

    electrum_v1 from_entropy(const data_chunk& entropy, language identifier) const
    {
        return electrum_v1::from_entropy(entropy, identifier);
    }

    electrum_v1 from_words(const string_list& words, language identifier) const
    {
        return electrum_v1::from_words(words, identifier);
    }
};

} // mnemonics_electrum_v1
} // test

#endif
