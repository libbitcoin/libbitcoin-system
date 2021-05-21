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

#include <string>
#include <vector>
#include <bitcoin/system.hpp>

struct electrum_v1_vector
{
    std::string entropy;
    std::string mnemonic;
    std::string passphrase;
    std::string seed;
};

typedef std::vector<electrum_v1_vector> electrum_v1_vectors;

// Electrum Version1 mnemonic validation algorithm.
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
//    for i in range(len(message)//8):
//        word = message[8*i:8*i+8]
//        x = int(word, 16)
//        w1 = (x%n)
//        w2 = ((x//n) + w1)%n
//        w3 = ((x//n//n) + w2)%n
//        out += [ words[w1], words[w2], words[w3] ]
//    return out
//
//def mn_decode( wlist ):
//    out = ''
//    for i in range(len(wlist)//3):
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

electrum_v1_vectors vectors
{
    electrum_v1_vector
    {
        "blind faith blind faith blind faith blind faith blind faith blind faith",
        "c6e913bc397bef94c6e913bc397bef94",
        "",
        ""
    },
    electrum_v1_vector
    {
        "blind faith blind faith blind faith blind faith blind faith blind faith "
        "blind faith blind faith blind faith blind faith blind faith blind faith",
        "c6e913bc397bef94c6e913bc397bef94c6e913bc397bef94c6e913bc397bef94",
        "",
        ""
    }
};

const auto sentence2 = "blind faith";

// mixed valid languages
const auto mixed_sentence12 =
    "abissinio faith blind faith blind faith "
    "blind faith blind faith blind faith";

// trimmable whitespace
const auto trimmable_sentence12 =
    "\tblind \nfaith \vblind faith blind faith \x20 "
    "blind faith blind faith blind\f faith\r";

const auto sentence12 =
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith";

const auto sentence24 =
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith";

const auto sentence26 =
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith";

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


const string_list words12
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

const string_list words24
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

const string_list words26
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith"
};

#endif
