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

typedef electrum_v1::bit_vector overflow_bits;

struct electrum_v1_vector
{
    language lingo;
    std::string mnemonic;
    data_chunk entropy;
    ec_uncompressed key;
    context network;

    string_list words() const
    {
        return split(mnemonic);
    }

    // Overflow bug testing.

    overflow_bits overflows_;
    std::string overflow_mnemonic_;

    bool is_overflow() const
    {
        return !overflow_mnemonic_.empty();
    }

    overflow_bits overflows() const
    {
        return is_overflow() ? overflows_ : overflow_bits(words().size() / 3u);
    }

    std::string entropy_mnemonic() const
    {
        return is_overflow() ? overflow_mnemonic_ : mnemonic;
    }

    string_list entropy_words() const
    {
        return split(entropy_mnemonic());
    }
};

typedef std::vector<electrum_v1_vector> electrum_v1_vectors;

// Vectors verified using: electrum/old_mnemonic.py
electrum_v1_vectors vectors_electrum
{
    electrum_v1_vector
    {
        // github.com/spesmilo/electrum/issues/3149
        language::en,
        "hurry idiot prefer sunset mention mist jaw inhale impossible kingdom rare squeeze",
        // This entropy is trimmed for overflow, does not round trip.
        base16_chunk("025d2f2d00503691003ca78900ca155c"),
        // This mpk is internally generated, adjusted for overflow.
        base16_array("04""0d6d957236f4e64467cbc4d3e835955eaea1c6f20232d549d0ba4475f4adb30509f5b04df03b3827c344734185fee0b3e7a978d8f40b2a25e35248c19be79799"),
        btc_mainnet_p2kh,
        // [025d2f2d][00503691][(1)003ca789][00ca155c]
        overflow_bits{ false, false, true, false },
        "hurry idiot prefer sunset mention mist ship baby bright kingdom rare squeeze",
    },
    electrum_v1_vector
    {
        // electrum/tests/test_wallet_vertical.py
        language::en,
        "alone body father children lead goodbye phone twist exist grass kick join",
        // This entropy is trimmed for overflow, does not round trip.
        base16_chunk("14039a7400162d9d0cc3c31f00168ddc"),
        // This is the reference mpk for the invalid seed, not the trimmed entropy.
        base16_array("04""cd805ed20aec61c7a8b409c121c6ba60a9221f46d20edbc2be83ebd91460e97937cd7d782e77c1cb08364c6bc1c98bc040fdad53f22f29f7d3a85c8e51f9c875"),
        btc_mainnet_p2kh,
        // [14039a74][(1)00162d9d][0cc3c31f][(1)00168ddc]
        overflow_bits{ false, true, false, true },
        "alone body father alas forgot forgot phone twist exist ask unable unable"
    },
    electrum_v1_vector
    {
        // electrum/tests/test_wallet_vertical.py
        language::en,
        "powerful random nobody notice nothing important anyway look away hidden message over",
        base16_chunk("acb740e454c3134901d7c8f16497cc1c"),
        base16_array("04""e9d4b7866dd1e91c862aebf62a49548c7dbf7bcc6e4b7b8c9da820c7737968df9c09d5a3e271dc814a29981f81b3faaf2737b551ef5dcc6189cf0f8252c442b3"),
        btc_mainnet_p2kh,
        // [acb740e4][54c31349][01d7c8f1][6497cc1c]
        overflow_bits{ false, false, false, false },
        ""
    }
};

// Vectors generated using: electrum/old_mnemonic.py
electrum_v1_vectors vectors_local
{
    electrum_v1_vector
    {
        // Minium value and length entropy.
        language::en,
        "like like like like like like like like like like like like",
        base16_chunk("00000000000000000000000000000000"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Minium value and maximum length entropy.
        language::en,
        "like like like like like like like like like like like like like like like like like like like like like like like like",
        base16_chunk("0000000000000000000000000000000000000000000000000000000000000000"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Maximum value for minimum length entropy.
        language::en,
        "fail husband howl fail husband howl fail husband howl fail husband howl",
        base16_chunk("ffffffffffffffffffffffffffffffff"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Maximum value and maximum length entropy.
        language::en,
        "fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl fail husband howl",
        base16_chunk("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Same words will never overflow (no distance).
        language::en,
        "foe foe foe foe foe foe foe foe foe foe foe foe",
        base16_chunk("00000630000006300000063000000630"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Double length of previous.
        language::en,
        "foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe foe",
        base16_chunk("0000063000000630000006300000063000000630000006300000063000000630"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Half of previous, switch to pt.
        language::pt,
        "vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito",
        base16_chunk("00000630000006300000063000000630"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Double length of previous.
        language::pt,
        "vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito vomito",
        base16_chunk("0000063000000630000006300000063000000630000006300000063000000630"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // This was a lucky guess non-overflow, words are close together.
        language::en,
        "blind faith blind faith blind faith blind faith blind faith blind faith",
        base16_chunk("c6e913bc397bef94c6e913bc397bef94"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Double length of previous.
        language::en,
        "blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith blind faith",
        base16_chunk("c6e913bc397bef94c6e913bc397bef94c6e913bc397bef94c6e913bc397bef94"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Another lucky guess non-overflow.
        language::pt,
        "gear ansioso medusa bicuspide lauto ciatico jesus doping iceberg empuxo harpista feixe",
        base16_chunk("85bd8950a3534bf1367b98dbeafc4022"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    },
    electrum_v1_vector
    {
        // Double length of previous and switch to en.
        language::en,
        "bite start agree mother buy dear swallow carry knock parent gift offer bite start agree mother buy dear swallow carry knock parent gift offer",
        base16_chunk("85bd8950a3534bf1367b98dbeafc402285bd8950a3534bf1367b98dbeafc4022"),
        base16_array("04""00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000")
    } 
};

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

// valid (non-overflow)
const string_list words12
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

// valid (non-overflow)
const string_list words24
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

// valid (two overflows)
const string_list two_overflows12
{
    "blind", "faith", "blind", "jaw", "inhale", "impossible",
    "blind", "faith", "blind", "jaw", "inhale", "impossible"
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

    static electrum_v1::result decoder(const string_list& words,
        language identifier)
    {
        return electrum_v1::decoder(words, identifier);
    }

    static string_list encoder(const data_chunk& entropy,
        language identifier)
    {
        return electrum_v1::encoder(entropy, identifier);
    }

    electrum_v1 from_entropy(const data_chunk& entropy,
        language identifier) const
    {
        return electrum_v1::from_entropy(entropy, identifier);
    }

    electrum_v1 from_words(const string_list& words,
        language identifier) const
    {
        return electrum_v1::from_words(words, identifier);
    }
};

} // mnemonics_electrum_v1
} // test

#endif
