/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(block_tests)

using namespace system::chain;

constexpr auto previous = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
constexpr auto merkle = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");

static const chain::header header
{
    10,
    previous,
    merkle,
    531234,
    6523454,
    68644
};

class accessor
  : public block
{
public:
    using block::block;
    using block::malleated32_size;
    using block::is_malleated32;
    using block::is_malleable32;
};

struct txs
{
    static transaction tx60() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{}, 42 } },
            outputs{ { 42, script{} } },
            42
        };
    }

    static transaction tx61() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{ { opcode::dup } }, 42 } },
            outputs{ { 42, script{} } },
            42
        };
    }

    static transaction tx62() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{ { opcode::dup } }, 42 } },
            outputs{ { 42, script{ { opcode::dup } } } },
            42
        };
    }

    static transaction tx63() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{ { opcode::dup, opcode::dup } }, 42 } },
            outputs{ { 42, script{ { opcode::dup } } } },
            42
        };
    }

    static transaction tx64() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{ { opcode::dup, opcode::dup } }, 42 } },
            outputs{ { 42, script{ { opcode::dup, opcode::dup } } } },
            42
        };
    }

    static transaction tx65() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{ { opcode::dup, opcode::dup, opcode::dup } }, 42 } },
            outputs{ { 42, script{ { opcode::dup, opcode::dup } } } },
            42
        };
    }

    static transaction tx66() NOEXCEPT
    {
        return
        {
            42,
            inputs{ { point{}, script{ { opcode::dup, opcode::dup, opcode::dup } }, 42 } },
            outputs{ { 42, script{ { opcode::dup, opcode::dup, opcode::dup } } } },
            42
        };
    }
};

BOOST_AUTO_TEST_CASE(block__transactions__sizes__expected)
{
    BOOST_REQUIRE_EQUAL(txs::tx60().serialized_size(false), 60u);
    BOOST_REQUIRE_EQUAL(txs::tx61().serialized_size(false), 61u);
    BOOST_REQUIRE_EQUAL(txs::tx62().serialized_size(false), 62u);
    BOOST_REQUIRE_EQUAL(txs::tx63().serialized_size(false), 63u);
    BOOST_REQUIRE_EQUAL(txs::tx64().serialized_size(false), 64u);
    BOOST_REQUIRE_EQUAL(txs::tx65().serialized_size(false), 65u);
    BOOST_REQUIRE_EQUAL(txs::tx66().serialized_size(false), 66u);
}

BOOST_AUTO_TEST_CASE(block__malleable__empty__false)
{
    const accessor instance{};
    BOOST_REQUIRE(!instance.is_malleable());
    BOOST_REQUIRE(!instance.is_malleable64());
    BOOST_REQUIRE(!instance.is_malleable32());
    BOOST_REQUIRE(!instance.is_malleable32(0, 0));
    BOOST_REQUIRE(!instance.is_malleated32());
    BOOST_REQUIRE(!instance.is_malleated32(0));
}

// is_malleable

BOOST_AUTO_TEST_CASE(block__is_malleable__64_not_32__true)
{
    const accessor instance{ header, { txs::tx64() } };
    BOOST_REQUIRE(instance.is_malleable());
    BOOST_REQUIRE(instance.is_malleable64());
    BOOST_REQUIRE(!instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable__32_not_64__true)
{
    const accessor instance{ header, { txs::tx60(), txs::tx61(), txs::tx62(), txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(instance.is_malleable());
    BOOST_REQUIRE(!instance.is_malleable64());
    BOOST_REQUIRE(instance.is_malleable32());
}

// is_malleable32

BOOST_AUTO_TEST_CASE(block__is_malleable32__one_64__false)
{
    const accessor instance{ header, { txs::tx64() } };
    BOOST_REQUIRE(!instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__two__false)
{
    const accessor instance{ header, { txs::tx64(), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__three__true)
{
    const accessor instance{ header, { txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__four__false)
{
    const accessor instance{ header, { txs::tx63(), txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(!instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__five__true)
{
    const accessor instance{ header, { txs::tx62(), txs::tx63(), txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__six__true)
{
    const accessor instance{ header, { txs::tx61(), txs::tx62(), txs::tx63(), txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(instance.is_malleable32());
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__six_duplicates__true)
{
    const accessor instance{ header, { txs::tx61(), txs::tx62(), txs::tx63(), txs::tx62(), txs::tx65(), txs::tx62() } };
    BOOST_REQUIRE(instance.is_malleable32());
}

// is_malleated32

BOOST_AUTO_TEST_CASE(block__is_malleated32__one_64__false)
{
    const accessor instance{ header, { txs::tx64() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__two_distinct__false)
{
    const accessor instance{ header, { txs::tx64(), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__two_same__false)
{
    const accessor instance{ header, { txs::tx65(), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__three_two_duplicated__false)
{
    const accessor instance{ header, { txs::tx64(), txs::tx65(), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__four_distinct__false)
{
    const accessor instance{ header, { txs::tx63(), txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__four_two_duplicated__false)
{
    const accessor instance{ header, { txs::tx63(), txs::tx64(), txs::tx63(), txs::tx64() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__five_two_duplicated__false)
{
    const accessor instance{ header, { txs::tx62(), txs::tx63(), txs::tx64(), txs::tx66(), txs::tx66() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__six_distinct__false)
{
    const accessor instance{ header, { txs::tx61(), txs::tx62(), txs::tx63(), txs::tx64(), txs::tx65(), txs::tx66() } };
    BOOST_REQUIRE(!instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__six_one_duplicated__true)
{
    const accessor instance{ header, { txs::tx61(), txs::tx62(), txs::tx63(), txs::tx64(), txs::tx65(), txs::tx65() } };
    BOOST_REQUIRE(instance.is_malleated32());
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__eight_two_duplicated__true)
{
    const accessor instance{ header, { txs::tx60(), txs::tx61(), txs::tx62(), txs::tx63(), txs::tx64(), txs::tx65(), txs::tx64(), txs::tx65() } };
    BOOST_REQUIRE(instance.is_malleated32());
}

// is_malleable64

BOOST_AUTO_TEST_CASE(block__is_malleable64__one_64__true)
{
    const accessor instance{ header, { txs::tx64() } };
    BOOST_REQUIRE(instance.is_malleable64());
}

BOOST_AUTO_TEST_CASE(block__is_malleable64__one_65__false)
{
    const accessor instance{ header, { txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleable64());
}

BOOST_AUTO_TEST_CASE(block__is_malleable64__two_64__true)
{
    const accessor instance{ header, { txs::tx64(), txs::tx64() } };
    BOOST_REQUIRE(instance.is_malleable64());
}

BOOST_AUTO_TEST_CASE(block__is_malleable64__two_64_65__false)
{
    const accessor instance{ header, { txs::tx64(), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleable64());
}

BOOST_AUTO_TEST_CASE(block__is_malleable64__three_64_65_64__false)
{
    const accessor instance{ header, { txs::tx64(), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleable64());
}

BOOST_AUTO_TEST_CASE(block__is_malleable64__three_64_64_64__true)
{
    const accessor instance{ header, { txs::tx64(), txs::tx64(), txs::tx64() } };
    BOOST_REQUIRE(instance.is_malleable64());
}

// is_malleable32

BOOST_AUTO_TEST_CASE(block__is_malleable32__overflow__false)
{
    BOOST_REQUIRE(!accessor::is_malleable32(0, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(1, 50));
    BOOST_REQUIRE(!accessor::is_malleable32(2, 100));
}

BOOST_AUTO_TEST_CASE(block__is_malleable32__various__expected)
{
    BOOST_REQUIRE(!accessor::is_malleable32(1, 1));

    BOOST_REQUIRE(!accessor::is_malleable32(2, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(2, 2));

    BOOST_REQUIRE( accessor::is_malleable32(3, 1)); // 4:1
    BOOST_REQUIRE(!accessor::is_malleable32(3, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(3, 3));

    BOOST_REQUIRE(!accessor::is_malleable32(4, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(4, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(4, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(4, 4));

    BOOST_REQUIRE( accessor::is_malleable32(5, 1)); // 6:1
    BOOST_REQUIRE(!accessor::is_malleable32(5, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(5, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(5, 4));
    BOOST_REQUIRE(!accessor::is_malleable32(5, 5));

    BOOST_REQUIRE(!accessor::is_malleable32(6, 1));
    BOOST_REQUIRE( accessor::is_malleable32(6, 2)); // 8:2
    BOOST_REQUIRE(!accessor::is_malleable32(6, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(6, 4));
    BOOST_REQUIRE(!accessor::is_malleable32(6, 5));
    BOOST_REQUIRE(!accessor::is_malleable32(6, 6));

    BOOST_REQUIRE( accessor::is_malleable32(7, 1)); // 8:1
    BOOST_REQUIRE(!accessor::is_malleable32(7, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(7, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(7, 4));
    BOOST_REQUIRE(!accessor::is_malleable32(7, 5));
    BOOST_REQUIRE(!accessor::is_malleable32(7, 6));
    BOOST_REQUIRE(!accessor::is_malleable32(7, 7));

    BOOST_REQUIRE(!accessor::is_malleable32(8, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 4));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 5));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 6));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 7));
    BOOST_REQUIRE(!accessor::is_malleable32(8, 8));

    BOOST_REQUIRE( accessor::is_malleable32(9, 1)); // 10:1
    BOOST_REQUIRE(!accessor::is_malleable32(9, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 4));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 5));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 6));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 7));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 8));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 9));

    BOOST_REQUIRE(!accessor::is_malleable32(10, 1));
    BOOST_REQUIRE( accessor::is_malleable32(10, 2)); // 12:2
    BOOST_REQUIRE(!accessor::is_malleable32(10, 3));
    BOOST_REQUIRE( accessor::is_malleable32(11, 1)); // 12:1
    BOOST_REQUIRE(!accessor::is_malleable32(11, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(12, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(12, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(12, 3));
    BOOST_REQUIRE( accessor::is_malleable32(12, 4)); // 16:4
    BOOST_REQUIRE( accessor::is_malleable32(13, 1)); // 14:1
    BOOST_REQUIRE(!accessor::is_malleable32(13, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(14, 1));
    BOOST_REQUIRE( accessor::is_malleable32(14, 2)); // 16:2
    BOOST_REQUIRE(!accessor::is_malleable32(14, 3));
    BOOST_REQUIRE( accessor::is_malleable32(15, 1)); // 16:1
    BOOST_REQUIRE(!accessor::is_malleable32(16, 1));
    BOOST_REQUIRE( accessor::is_malleable32(17, 1)); // 18:1
    BOOST_REQUIRE(!accessor::is_malleable32(18, 1));
    BOOST_REQUIRE( accessor::is_malleable32(18, 2)); // 20:2
    BOOST_REQUIRE( accessor::is_malleable32(19, 1)); // 20:1
    BOOST_REQUIRE(!accessor::is_malleable32(20, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(20, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(20, 3));
    BOOST_REQUIRE( accessor::is_malleable32(20, 4)); // 24:4
    BOOST_REQUIRE( accessor::is_malleable32(21, 1)); // 22:1
    BOOST_REQUIRE(!accessor::is_malleable32(22, 1));
    BOOST_REQUIRE( accessor::is_malleable32(22, 2)); // 24:2
    BOOST_REQUIRE( accessor::is_malleable32(23, 1)); // 24:1
    BOOST_REQUIRE(!accessor::is_malleable32(24, 1));
    BOOST_REQUIRE( accessor::is_malleable32(25, 1)); // 26:1
    BOOST_REQUIRE(!accessor::is_malleable32(26, 1));
    BOOST_REQUIRE( accessor::is_malleable32(26, 2)); // 28:2
    BOOST_REQUIRE( accessor::is_malleable32(27, 1)); // 28:1
    BOOST_REQUIRE(!accessor::is_malleable32(28, 1));
    BOOST_REQUIRE(!accessor::is_malleable32(28, 2));
    BOOST_REQUIRE(!accessor::is_malleable32(28, 3));
    BOOST_REQUIRE( accessor::is_malleable32(28, 4)); // 32:4
    BOOST_REQUIRE( accessor::is_malleable32(29, 1)); // 30:1
    BOOST_REQUIRE(!accessor::is_malleable32(30, 1));
    BOOST_REQUIRE( accessor::is_malleable32(30, 2)); // 32:2
    BOOST_REQUIRE( accessor::is_malleable32(31, 1)); // 32:1
}

BOOST_AUTO_TEST_SUITE_END()
