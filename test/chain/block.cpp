/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

namespace json = boost::json;
using namespace system::chain;

constexpr auto hash1 = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
constexpr auto hash2 = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
constexpr auto hash3 = base16_hash("bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");

static const header expected_header
{
    10,
    hash1,
    hash2,
    531234,
    6523454,
    68644
};

struct expected_transactions
{
    static const transactions& get() NOEXCEPT
    {
        static transactions txs_
        {
            { 1, inputs{ {} }, { {} }, 48 },
            { 2, inputs{ {} }, { {} }, 32 },
            { 4, inputs{ {} }, { {} }, 16 }
        };

        return txs_;
    }
};

struct expected_block
{
    static const block& get() NOEXCEPT
    {
        static block block_{ expected_header, expected_transactions::get() };
        return block_;
    }

    static const data_chunk& data() NOEXCEPT
    {
        static data_chunk chunk_{ get().to_data(true) };
        return chunk_;
    }
};

// Access protected validation methods.
class accessor
  : public block
{
public:
    // Use base class constructors.
    using block::block;

    bool is_empty() const NOEXCEPT
    {
        return block::is_empty();
    }

    bool is_oversized() const NOEXCEPT
    {
        return block::is_oversized();
    }

    bool is_first_non_coinbase() const NOEXCEPT
    {
        return block::is_first_non_coinbase();
    }

    bool is_extra_coinbases() const NOEXCEPT
    {
        return block::is_extra_coinbases();
    }

    bool is_forward_reference() const NOEXCEPT
    {
        return block::is_forward_reference();
    }

    bool is_internal_double_spend() const NOEXCEPT
    {
        return block::is_internal_double_spend();
    }

    bool is_invalid_merkle_root() const NOEXCEPT
    {
        return block::is_invalid_merkle_root();
    }

    bool is_overweight() const NOEXCEPT
    {
        return block::is_overweight();
    }

    bool is_invalid_coinbase_script(size_t height) const NOEXCEPT
    {
        return block::is_invalid_coinbase_script(height);
    }

    bool is_hash_limit_exceeded() const NOEXCEPT
    {
        return block::is_hash_limit_exceeded();
    }

    bool is_invalid_witness_commitment() const NOEXCEPT
    {
        return block::is_invalid_witness_commitment();
    }

    bool is_overspent(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT
    {
        return block::is_overspent(height, subsidy_interval,
            initial_block_subsidy_satoshi, bip42);
    }

    size_t is_signature_operations_limited(bool bip16, bool bip141) const NOEXCEPT
    {
        return block::is_signature_operations_limited(bip16, bip141);
    }

    bool is_unspent_coinbase_collision() const NOEXCEPT
    {
        return block::is_unspent_coinbase_collision();
    }
};

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BOOST_AUTO_TEST_CASE(block__constructor__default__invalid)
{
    const block instance{};
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(block__constructor__move__expected)
{
    const block expected(expected_header, expected_transactions::get());
    block copy(expected_header, expected_transactions::get());
    const block instance(std::move(copy));
    BOOST_REQUIRE(instance == instance);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__constructor__copy__expected)
{
    const block expected(expected_header, expected_transactions::get());
    const block instance(expected);
    BOOST_REQUIRE(instance == instance);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__constructor__move_parameters__expected)
{
    auto header = expected_header;
    auto transactions = expected_transactions::get();
    const block instance(std::move(header), std::move(transactions));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());

}

BOOST_AUTO_TEST_CASE(block__constructor__copy_parameters__expected)
{
    const auto header = expected_header;
    const auto& transactions = expected_transactions::get();
    const block instance(header, transactions);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__constructor__data__expected)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);
    const accessor block(data, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

const block& get_block() NOEXCEPT
{
    constexpr auto root = base16_hash("0f3842895590aae2727ca1b35c755d1d9e577a8f2640d972f9989bdcddc9c1e5");
    constexpr uint32_t block_version{ 0 };
    constexpr hash_digest previous_block_hash{ 1 };
    constexpr hash_digest merkle_root{ root };
    constexpr uint32_t timestamp{ 3 };
    constexpr uint32_t bits{ 4 };
    constexpr uint32_t nonce{ 5 };
    constexpr uint32_t tx_version{ 6 };
    constexpr uint32_t locktime{ 7 };
    constexpr uint32_t sequence{ 8 };
    constexpr uint64_t value{ 9 };
    constexpr bool minimal{ true };

    static const block instance
    {
        header // ptr<header>
        {
            block_version,
            previous_block_hash,
            merkle_root,
            timestamp,
            bits,
            nonce
        },
        transactions // ptr<vector<tx>>
        {
            // coinbase
            transaction  // first tx
            {
                tx_version,
                inputs
                {
                    input // exactly one input
                    {
                        point // null point
                        {
                            null_hash,
                            point::null_index
                        },
                        script // minumum 2 bytes
                        {
                            operations // vector
                            {
                                operation
                                {
                                    opcode::nop1 // ptr<nullptr>
                                },
                                operation
                                {
                                    opcode::nop2 // ptr<nullptr>
                                },
                                operation
                                {
                                    data_chunk{ 0x42 , 0x24 }, // ptr<data_chunk>
                                    minimal
                                },
                                operation
                                {
                                    data_chunk{ 0xfe , 0xed }, // ptr<data_chunk>
                                    minimal
                                }
                            }
                        },
                        witness
                        {
                        },
                        sequence
                    },
                    input
                    {
                        point    // ptr<point>
                        {
                            null_hash,
                            point::null_index
                        },
                        script   // ptr<script>
                        {
                        },
                        witness  // ptr<witness>
                        {
                            data_stack // vector
                            {
                                data_chunk{}, // ptr<data_chunk>
                                data_chunk{}  // ptr<data_chunk>
                            }
                        },
                        sequence
                    }
                },
                outputs
                {
                    output // at least one output
                    {
                        value,  // no more than reward
                        script{}
                    },
                    output // at least one output
                    {
                        value,  // no more than reward
                        script{}
                    }
                },
                locktime
            }
        }
    };

    return instance;
}

BOOST_AUTO_TEST_CASE(block__constructor__fast__success)
{
    const auto block1 = get_block();
    const auto data = block1.to_data(true);
    test::reporting_arena<false> arena{};
    stream::in::fast stream(data);
    read::bytes::fast source(stream, &arena);
    const accessor block(source, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__constructor__stream__success)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);
    stream::in::copy stream(data);
    const accessor block(stream, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__constructor__reader__success)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);
    read::bytes::copy source(data);
    const accessor block(source, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__assign__move__expected)
{
    const auto& alpha = expected_block::get();
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(block__assign__copy__expected)
{
    const auto& alpha = expected_block::get();
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(block__equality__same__true)
{
    const auto& alpha = expected_block::get();
    const block beta(alpha);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(block__equality__different__false)
{
    const auto& alpha = expected_block::get();
    const block beta;
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(block__inequality__same__false)
{
    const auto& alpha = expected_block::get();
    const block beta(alpha);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(block__inequality__different__true)
{
    const auto& alpha = expected_block::get();
    const block beta;
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__to_data__data__expected)
{
    const auto size = expected_block::data().size();
    BOOST_REQUIRE_EQUAL(size, expected_block::get().serialized_size(true));
}

BOOST_AUTO_TEST_CASE(block__to_data__stream__expected)
{
    // Write block to stream.
    std::stringstream iostream{};
    expected_block::get().to_data(iostream, true);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const block copy(iostream, true);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__to_data__writer__expected)
{
    // Write block to stream.
    std::stringstream iostream{};
    write::bytes::ostream out(iostream);
    expected_block::get().to_data(out, true);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const block copy(iostream, true);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_block::get());
}

// properties
// ----------------------------------------------------------------------------

// weight
// fees
// claim

BOOST_AUTO_TEST_CASE(block__spends__genesis__zero)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    BOOST_REQUIRE(is_zero(genesis.spends()));
}

BOOST_AUTO_TEST_CASE(block__spends__coinbase_only__zero)
{
    BOOST_REQUIRE(is_zero(get_block().spends()));
}

BOOST_AUTO_TEST_CASE(block__hash__default__matches_header_hash)
{
    const block instance;
    BOOST_REQUIRE_EQUAL(instance.hash(), instance.header().hash());
}

// is_malleable
// is_segregated
// serialized_size

// validation (public)
// ----------------------------------------------------------------------------

// check
// accept
// connect

// validation (protected)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__is_empty__default__true)
{
    const accessor instance;
    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(block__is_empty__single_transaction__false)
{
    const accessor instance
    {
        header{},
        { {} }
    };

    BOOST_REQUIRE(!instance.is_empty());
}

// is_oversized
// is_first_non_coinbase
// is_extra_coinbases

BOOST_AUTO_TEST_CASE(block__is_forward_reference__empty__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__multiple_empty_transactions__false)
{
    const accessor instance
    {
        {},
        {
            { 0, inputs{}, {}, 0 },
            { 0, inputs{}, {}, 0 }
        }
    };

    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__duplicate_transactions__false)
{
    const accessor instance
    {
        {},
        {
            { 0, inputs{}, {}, 0 },
            { 0, inputs{}, {}, 0 }
        }
    };

    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__backward_reference__false)
{
    const transaction to{ 0, inputs{}, {}, 0 };
    const transaction from{ 0, { { { to.hash(false), 0 }, {}, 0 } }, {}, 0 };
    const accessor instance
    {
        {},
        {
            to,
            from
        }
    };

    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__forward_reference__true)
{
    const transaction cb{ 0, inputs{}, {}, 0 };
    const transaction to{ 0, inputs{}, {}, 42 };
    const transaction from{ 0, { { { to.hash(false), 0 }, {}, 0 } }, {}, 0 };
    const accessor instance
    {
        {},
        {
            cb,
            from,
            to
        }
    };

    BOOST_REQUIRE(instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_internal_double_spend__empty__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(block__is_internal_double_spend__distinct_points__false)
{
    const accessor instance
    {
        {},
        {
            {},
            { 0, { { { hash1, 42 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash1, 27 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash2, 42 }, {}, 0 } }, {}, 0 }
        }
    };

    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(block__is_internal_double_spend__nondistinct_points__true)
{
    const accessor instance
    {
        {},
        {
            {},
            { 0, { { { hash1, 42 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash2, 27 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash3, 36 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash1, 42 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash2, 27 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash3, 36 }, {}, 0 } }, {}, 0 }
        }
    };

    BOOST_REQUIRE(instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__default__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__default_header_non_empty__true)
{
    const accessor instance
    {
        header{},
        { {} }
    };

    BOOST_REQUIRE(instance.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__mainnet_genesis__false)
{
    const auto data = settings(selection::mainnet).genesis_block.to_data(true);
    const accessor genesis(data, true);
    BOOST_REQUIRE(genesis.is_valid());
    BOOST_REQUIRE(!genesis.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__testnet_genesis__false)
{
    const auto data = settings(selection::testnet).genesis_block.to_data(true);
    const accessor genesis(data, true);
    BOOST_REQUIRE(genesis.is_valid());
    BOOST_REQUIRE(!genesis.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__regtest_genesis__false)
{
    const auto data = settings(selection::regtest).genesis_block.to_data(true);
    const accessor genesis(data, true);
    BOOST_REQUIRE(genesis.is_valid());
    BOOST_REQUIRE(!genesis.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__valid__false)
{
    constexpr auto data = base16_array(
        "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a0000000"
        "0005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4dc118244d6"
        "7fb74c9d8e2f1bea5ee82a03010000000100000000000000000000000000000000000"
        "00000000000000000000000000000ffffffff07049d8e2f1b0114ffffffff0100f205"
        "2a0100000043410437b36a7221bc977dce712728a954e3b5d88643ed5aef46660ddcf"
        "eeec132724cd950c1fdd008ad4a2dfd354d6af0ff155fc17c1ee9ef802062feb07ef1"
        "d065f0ac000000000100000001260fd102fab456d6b169f6af4595965c03c2296ecf2"
        "5bfd8790e7aa29b404eff010000008c493046022100c56ad717e07229eb93ecef2a32"
        "a42ad041832ffe66bd2e1485dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1"
        "312634664bac46f36ddd35761edaae20cefb16f01410417e418ba79380f462a60d8dd"
        "12dcef8ebfd7ab1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583"
        "040b1bc341b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914"
        "fc7b44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a9146"
        "17f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122cd6da2"
        "6eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb013010000008c493"
        "0460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d15774594bfedc45c4f9"
        "9e2f022100ae0135094a7d651801539df110a028d65459d24bc752d7512bc8a9f78b4"
        "ab368014104a2e06c38dc72c4414564f190478e3b0d01260f09b8520b196c2f6ec3d0"
        "6239861e49507f09b7568189efe8d327c3384a4e488f8c534484835f8020b3669e5ae"
        "bffffffff0200ac23fc060000001976a914b9a2c9700ff9519516b21af338d28d53dd"
        "f5349388ac00743ba40b0000001976a914eb675c349c474bec8dea2d79d12cff6f330"
        "ab48788ac00000000");

    const accessor instance(data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__block100k__false)
{
    constexpr auto data = base16_array(
        "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a00"
        "000000005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4"
        "dc118244d67fb74c9d8e2f1bea5ee82a03010000000100000000000000000000"
        "00000000000000000000000000000000000000000000ffffffff07049d8e2f1b"
        "0114ffffffff0100f2052a0100000043410437b36a7221bc977dce712728a954"
        "e3b5d88643ed5aef46660ddcfeeec132724cd950c1fdd008ad4a2dfd354d6af0"
        "ff155fc17c1ee9ef802062feb07ef1d065f0ac000000000100000001260fd102"
        "fab456d6b169f6af4595965c03c2296ecf25bfd8790e7aa29b404eff01000000"
        "8c493046022100c56ad717e07229eb93ecef2a32a42ad041832ffe66bd2e1485"
        "dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1312634664bac46f36dd"
        "d35761edaae20cefb16f01410417e418ba79380f462a60d8dd12dcef8ebfd7ab"
        "1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583040b1bc34"
        "1b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914fc7b"
        "44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a914"
        "617f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122"
        "cd6da26eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb01301"
        "0000008c4930460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d157"
        "74594bfedc45c4f99e2f022100ae0135094a7d651801539df110a028d65459d2"
        "4bc752d7512bc8a9f78b4ab368014104a2e06c38dc72c4414564f190478e3b0d"
        "01260f09b8520b196c2f6ec3d06239861e49507f09b7568189efe8d327c3384a"
        "4e488f8c534484835f8020b3669e5aebffffffff0200ac23fc060000001976a9"
        "14b9a2c9700ff9519516b21af338d28d53ddf5349388ac00743ba40b00000019"
        "76a914eb675c349c474bec8dea2d79d12cff6f330ab48788ac00000000");

    const accessor instance(data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_invalid_merkle_root());
}

// is_overweight
// is_invalid_coinbase_script
// is_hash_limit_exceeded
// is_invalid_witness_commitment
// is_overspent
// is_signature_operations_limited
// is_unspent_coinbase_collision

// json
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__json__conversions__expected)
{
    const std::string text
    {
        "{"
            "\"header\":"
            "{"
                "\"version\":42,"
                "\"previous\":\"0000000000000000000000000000000000000000000000000000000000000000\","
                "\"merkle_root\":\"0000000000000000000000000000000000000000000000000000000000000001\","
                "\"timestamp\":43,"
                "\"bits\":44,"
                "\"nonce\":45"
            "},"
            "\"transactions\":"
            "["
                "{"
                    "\"version\":42,"
                    "\"inputs\":"
                    "["
                        "{"
                            "\"point\":"
                            "{"
                                "\"hash\":\"0000000000000000000000000000000000000000000000000000000000000000\","
                                "\"index\":24"
                            "},"
                            "\"script\":\"return pick\","
                            "\"witness\":\"[242424]\","
                            "\"sequence\":42"
                        "},"
                        "{"
                            "\"point\":"
                            "{"
                                "\"hash\":\"0000000000000000000000000000000000000000000000000000000000000001\","
                                "\"index\":42"
                            "},"
                            "\"script\":\"return roll\","
                            "\"witness\":\"[424242]\","
                            "\"sequence\":24"
                        "}"
                    "],"
                    "\"outputs\":"
                    "["
                        "{"
                            "\"value\":24,"
                            "\"script\":\"pick\""
                        "},"
                        "{"
                            "\"value\":42,"
                            "\"script\":\"roll\""
                        "}"
                    "],"
                    "\"locktime\":24"
                "}"
            "]"
        "}"
    };

    const chain::block instance
    {
        chain::header
        {
            42, null_hash, one_hash, 43, 44, 45
        },
        chain::transactions
        {
            chain::transaction
            {
                42,
                chain::inputs
                {
                    chain::input
                    {
                        chain::point{ null_hash, 24 },
                        chain::script{ { { opcode::op_return }, { opcode::pick } } },
                        chain::witness{ "[242424]" },
                        42
                    },
                    chain::input
                    {
                        chain::point{ one_hash, 42 },
                        chain::script{ { { opcode::op_return }, { opcode::roll } } },
                        chain::witness{ "[424242]" },
                        24
                    }
                },
                chain::outputs
                {
                    chain::output
                    {
                        24,
                        chain::script{ { { opcode::pick } } }
                    },
                    chain::output
                    {
                        42,
                        chain::script{ { { opcode::roll } } }
                    }
                },
                24
            }
        }
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE(json::parse(text) == value);
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<chain::block>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
