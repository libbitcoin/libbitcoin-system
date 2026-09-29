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

BOOST_AUTO_TEST_SUITE(block_malleable_tests)

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
    using block::malleated_or;
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
        const script three{ { opcode::dup, opcode::dup, opcode::dup } };
        const script two{ { opcode::dup, opcode::dup } };
        return
        {
            42,
            inputs{ { point{}, three, 42 } },
            outputs{ { 42, two } },
            42
        };
    }

    static transaction tx66() NOEXCEPT
    {
        const script three{ { opcode::dup, opcode::dup, opcode::dup } };
        return
        {
            42,
            inputs{ { point{}, three, 42 } },
            outputs{ { 42, three } },
            42
        };
    }
};

// Distinct transactions, none 64 bytes.
static transaction tx(uint32_t index) NOEXCEPT
{
    return
    {
        42,
        inputs{ { point{}, script{}, 42 } },
        outputs{ { 42, script{} } },
        index
    };
}

static transactions tx_set(const std::vector<uint32_t>& indexes) NOEXCEPT
{
    transactions txs{};
    for (const auto index: indexes)
        txs.push_back(tx(index));

    return txs;
}

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
    BOOST_REQUIRE(!block::is_malleated32({}, 0));
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

// Padded five is 0 1 2 3 4 4 4 4, its prefixes of six to eight share its root.
BOOST_AUTO_TEST_CASE(block__is_malleated32__five_one_trailing_clone__true)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 4 }) };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE_EQUAL(instance.malleated32_size(), 1u);
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__five_two_trailing_clones__true)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 4, 4 }) };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE_EQUAL(instance.malleated32_size(), 2u);
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__five_three_trailing_clones__true)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 4, 4, 4 }) };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE_EQUAL(instance.malleated32_size(), 1u);
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__nine_two_trailing_clones__true)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8 }) };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE_EQUAL(instance.malleated32_size(), 2u);
}

BOOST_AUTO_TEST_CASE(block__is_malleated32__nine_four_trailing_clones__true)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8, 8 }) };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE_EQUAL(instance.malleated32_size(), 4u);
}

// Padded ten is 0..9 8 9 8 9 8 9, the last pair is cloned.
BOOST_AUTO_TEST_CASE(block__is_malleated32__ten_two_trailing_pair_clones__true)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 9, 8, 9 }) };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE_EQUAL(instance.malleated32_size(), 4u);
}

// Not a prefix of any padded set.
BOOST_AUTO_TEST_CASE(block__is_malleated32__seven_two_distinct_trailing_clones__false)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 3, 4, 4 }) };
    BOOST_REQUIRE(!instance.is_malleated32());
}

// Nine leaves are a depth deeper than any set of eight or fewer.
BOOST_AUTO_TEST_CASE(block__is_malleated32__nine_five_trailing_clones__false)
{
    const accessor instance{ header, tx_set({ 0, 1, 2, 3, 4, 4, 4, 4, 4 }) };
    BOOST_REQUIRE(!instance.is_malleated32());
}

// Only the last node at a depth is cloned.
BOOST_AUTO_TEST_CASE(block__is_malleated32__eight_leading_pair_clone__false)
{
    const accessor instance{ header, tx_set({ 0, 1, 0, 1, 2, 3, 4, 5 }) };
    BOOST_REQUIRE(!instance.is_malleated32());
}

// merkle_index

BOOST_AUTO_TEST_CASE(block__merkle_index__one_of_one__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 1, 1), 0u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__one_of_two__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 1, 2), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 1, 2), 0u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__two_of_two__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 2, 2), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 2, 2), 1u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__three_of_four__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 3, 4), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 3, 4), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 3, 4), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 3, 4), 2u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__one_of_eight__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(4, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(5, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(6, 1, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(7, 1, 8), 0u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__three_of_eight__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 3, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 3, 8), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 3, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 3, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(4, 3, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(5, 3, 8), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(6, 3, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(7, 3, 8), 2u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__five_of_eight__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 5, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 5, 8), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 5, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 5, 8), 3u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(4, 5, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(5, 5, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(6, 5, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(7, 5, 8), 4u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__six_of_eight__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 6, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 6, 8), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 6, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 6, 8), 3u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(4, 6, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(5, 6, 8), 5u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(6, 6, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(7, 6, 8), 5u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__seven_of_eight__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 7, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 7, 8), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 7, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 7, 8), 3u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(4, 7, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(5, 7, 8), 5u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(6, 7, 8), 6u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(7, 7, 8), 6u);
}

BOOST_AUTO_TEST_CASE(block__merkle_index__eight_of_eight__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(0, 8, 8), 0u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(1, 8, 8), 1u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(2, 8, 8), 2u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(3, 8, 8), 3u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(4, 8, 8), 4u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(5, 8, 8), 5u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(6, 8, 8), 6u);
    BOOST_REQUIRE_EQUAL(accessor::merkle_index(7, 8, 8), 7u);
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

BOOST_AUTO_TEST_CASE(block__is_malleable32__not_power2_width__false)
{
    BOOST_REQUIRE(!accessor::is_malleable32(3, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(6, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(9, 3));
    BOOST_REQUIRE(!accessor::is_malleable32(12, 6));
}

// True where the node count (trailing comment) is odd and above one.
BOOST_AUTO_TEST_CASE(block__is_malleable32__various__expected)
{
    BOOST_REQUIRE(!accessor::is_malleable32(1, 1)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(2, 1)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(2, 2)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(3, 1)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(3, 2)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(3, 4)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(4, 1)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(4, 2)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(4, 4)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(5, 1)); // 5
    BOOST_REQUIRE( accessor::is_malleable32(5, 2)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(5, 4)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(5, 8)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(6, 1)); // 6
    BOOST_REQUIRE( accessor::is_malleable32(6, 2)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(6, 4)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(6, 8)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(7, 1)); // 7
    BOOST_REQUIRE(!accessor::is_malleable32(7, 2)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(7, 4)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(7, 8)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(8, 1)); // 8
    BOOST_REQUIRE(!accessor::is_malleable32(8, 2)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(8, 4)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(8, 8)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(9, 1)); // 9
    BOOST_REQUIRE( accessor::is_malleable32(9, 2)); // 5
    BOOST_REQUIRE( accessor::is_malleable32(9, 4)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(9, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(9, 16)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(10, 1)); // 10
    BOOST_REQUIRE( accessor::is_malleable32(10, 2)); // 5
    BOOST_REQUIRE( accessor::is_malleable32(10, 4)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(10, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(10, 16)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(11, 1)); // 11
    BOOST_REQUIRE(!accessor::is_malleable32(11, 2)); // 6
    BOOST_REQUIRE( accessor::is_malleable32(11, 4)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(11, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(11, 16)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(12, 1)); // 12
    BOOST_REQUIRE(!accessor::is_malleable32(12, 2)); // 6
    BOOST_REQUIRE( accessor::is_malleable32(12, 4)); // 3
    BOOST_REQUIRE(!accessor::is_malleable32(12, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(12, 16)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(13, 1)); // 13
    BOOST_REQUIRE( accessor::is_malleable32(13, 2)); // 7
    BOOST_REQUIRE(!accessor::is_malleable32(13, 4)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(13, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(13, 16)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(14, 1)); // 14
    BOOST_REQUIRE( accessor::is_malleable32(14, 2)); // 7
    BOOST_REQUIRE(!accessor::is_malleable32(14, 4)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(14, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(14, 16)); // 1

    BOOST_REQUIRE( accessor::is_malleable32(15, 1)); // 15
    BOOST_REQUIRE(!accessor::is_malleable32(15, 2)); // 8
    BOOST_REQUIRE(!accessor::is_malleable32(15, 4)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(15, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(15, 16)); // 1

    BOOST_REQUIRE(!accessor::is_malleable32(16, 1)); // 16
    BOOST_REQUIRE(!accessor::is_malleable32(16, 2)); // 8
    BOOST_REQUIRE(!accessor::is_malleable32(16, 4)); // 4
    BOOST_REQUIRE(!accessor::is_malleable32(16, 8)); // 2
    BOOST_REQUIRE(!accessor::is_malleable32(16, 16)); // 1
}

// is_malleated64
// ----------------------------------------------------------------------------
// Malleated64 additionally requires a non-null first input point.

static transaction tx64_spend(uint32_t index) NOEXCEPT
{
    const script two{ { opcode::dup, opcode::dup } };
    return
    {
        42,
        inputs{ { point{ one_hash, index }, two, 42 } },
        outputs{ { 42, two } },
        42
    };
}

BOOST_AUTO_TEST_CASE(block__is_malleated64__empty__false)
{
    const accessor instance{};
    BOOST_REQUIRE(!instance.is_malleated64());
    BOOST_REQUIRE(!instance.is_malleated());
}

BOOST_AUTO_TEST_CASE(block__is_malleated64__spending_first_input__true)
{
    const accessor instance{ header, { tx64_spend(0), tx64_spend(1) } };
    BOOST_REQUIRE_EQUAL(tx64_spend(0).serialized_size(false), 64u);
    BOOST_REQUIRE(instance.is_malleable64());
    BOOST_REQUIRE(instance.is_malleated64());
    BOOST_REQUIRE(instance.is_malleated());
}

BOOST_AUTO_TEST_CASE(block__is_malleated64__coinbase_first__false)
{
    const accessor instance{ header, { txs::tx64(), tx64_spend(1) } };
    BOOST_REQUIRE(instance.is_malleable64());
    BOOST_REQUIRE(!instance.is_malleated64());
    BOOST_REQUIRE(!instance.is_malleated());
}

BOOST_AUTO_TEST_CASE(block__is_malleated64__mixed_sizes__false)
{
    const accessor instance{ header, { tx64_spend(0), txs::tx65() } };
    BOOST_REQUIRE(!instance.is_malleable64());
    BOOST_REQUIRE(!instance.is_malleated64());
}

// is_malleated
// ----------------------------------------------------------------------------

// The composite is satisfied by the 32 byte shape alone.
BOOST_AUTO_TEST_CASE(block__is_malleated__malleated32_only__true)
{
    const accessor instance{ header, { txs::tx61(), txs::tx62(), txs::tx63(), txs::tx64(), txs::tx65(), txs::tx65() } };
    BOOST_REQUIRE(instance.is_malleated32());
    BOOST_REQUIRE(!instance.is_malleated64());
    BOOST_REQUIRE(instance.is_malleated());
}

BOOST_AUTO_TEST_CASE(block__is_malleated__neither_shape__false)
{
    const accessor instance{ header, { txs::tx60(), txs::tx61() } };
    BOOST_REQUIRE(!instance.is_malleated32());
    BOOST_REQUIRE(!instance.is_malleated64());
    BOOST_REQUIRE(!instance.is_malleated());
}

// malleated_or
// ----------------------------------------------------------------------------
// A malleated block is reported as a commitment failure.

BOOST_AUTO_TEST_CASE(block__malleated_or__unmalleated__given_code)
{
    const accessor instance{ header, { txs::tx60(), txs::tx61() } };
    BOOST_REQUIRE_EQUAL(instance.malleated_or(error::invalid_witness_commitment), error::invalid_witness_commitment);
    BOOST_REQUIRE_EQUAL(instance.malleated_or(error::block_success), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__malleated_or__malleated__invalid_transaction_commitment)
{
    const accessor instance{ header, { tx64_spend(0), tx64_spend(1) } };
    BOOST_REQUIRE(instance.is_malleated());
    BOOST_REQUIRE_EQUAL(instance.malleated_or(error::invalid_witness_commitment), error::invalid_transaction_commitment);
    BOOST_REQUIRE_EQUAL(instance.malleated_or(error::block_success), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_SUITE_END()
