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
#include "blocks.hpp"

// blockchain.info/rawblock/[hash]?format=hex

namespace test {

using namespace system;
constexpr hash_digest block0_hash = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
constexpr hash_digest block1_hash = base16_hash("00000000839a8e6886ab5951d76f411475428afc90947ee320161bbf18eb6048");
constexpr hash_digest block2_hash = base16_hash("000000006a625f06636b8bb6ac7b960a8d03705d1ace08b1a19da3fdcc99ddbd");
constexpr hash_digest block3_hash = base16_hash("0000000082b5015589a3fdf2d4baff403e6f0be035a5d9742c1cae6295464449");
constexpr hash_digest block4_hash = base16_hash("000000004ebadb55ee9096c9a2f8880e09da59c0d68b1c228da88e48844a1485");
constexpr hash_digest block5_hash = base16_hash("000000009b7262315dbf071787ad3656097b892abffd1f95a1a022f896f533fc");
constexpr hash_digest block6_hash = base16_hash("000000003031a0e73735690c5a1ff2a4be82553b2a12b776fbd3a215dc8f778d");
constexpr hash_digest block7_hash = base16_hash("0000000071966c2b1d065fd446b1e485b2c9d9594acd2007ccbd5441cfc89444");
constexpr hash_digest block8_hash = base16_hash("00000000408c48f847aa786c2268fc3e6ec2af68e8468a34a28c61b7f1de0dc6");
constexpr hash_digest block9_hash = base16_hash("000000008d9dc510f23c2657fc4f67bea30078cc05a90eb89e84cc475c080805");

constexpr header_data header0_data = base16_array("0100000000000000000000000000000000000000000000000000000000000000000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a29ab5f49ffff001d1dac2b7c");
constexpr header_data header1_data = base16_array("010000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d6190000000000982051fd1e4ba744bbbe680e1fee14677ba1a3c3540bf7b1cdb606e857233e0e61bc6649ffff001d01e36299");
constexpr header_data header2_data = base16_array("010000004860eb18bf1b1620e37e9490fc8a427514416fd75159ab86688e9a8300000000d5fdcc541e25de1c7a5addedf24858b8bb665c9f36ef744ee42c316022c90f9bb0bc6649ffff001d08d2bd61");
constexpr header_data header3_data = base16_array("01000000bddd99ccfda39da1b108ce1a5d70038d0a967bacb68b6b63065f626a0000000044f672226090d85db9a9f2fbfe5f0f9609b387af7be5b7fbb7a1767c831c9e995dbe6649ffff001d05e0ed6d");
constexpr header_data header4_data = base16_array("010000004944469562ae1c2c74d9a535e00b6f3e40ffbad4f2fda3895501b582000000007a06ea98cd40ba2e3288262b28638cec5337c1456aaf5eedc8e9e5a20f062bdf8cc16649ffff001d2bfee0a9");
constexpr header_data header5_data = base16_array("0100000085144a84488ea88d221c8bd6c059da090e88f8a2c99690ee55dbba4e00000000e11c48fecdd9e72510ca84f023370c9a38bf91ac5cae88019bee94d24528526344c36649ffff001d1d03e477");
constexpr header_data header6_data = base16_array("01000000fc33f596f822a0a1951ffdbf2a897b095636ad871707bf5d3162729b00000000379dfb96a5ea8c81700ea4ac6b97ae9a9312b2d4301a29580e924ee6761a2520adc46649ffff001d189c4c97");
constexpr header_data header7_data = base16_array("010000008d778fdc15a2d3fb76b7122a3b5582bea4f21f5a0c693537e7a03130000000003f674005103b42f984169c7d008370967e91920a6a5d64fd51282f75bc73a68af1c66649ffff001d39a59c86");
constexpr header_data header8_data = base16_array("010000004494c8cf4154bdcc0720cd4a59d9c9b285e4b146d45f061d2b6c967100000000e3855ed886605b6d4a99d5fa2ef2e9b0b164e63df3c4136bebf2d0dac0f1f7a667c86649ffff001d1c4b5666");
constexpr header_data header9_data = base16_array("01000000c60ddef1b7618ca2348a46e868afc26e3efc68226c78aa47f8488c4000000000c997a5e56e104102fa209c6a852dd90660a20b2d9c352423edce25857fcd37047fca6649ffff001d28404f53");

constexpr block_data block1_data = base16_array("010000006fe28c0ab6f1b372c1a6a246ae63f74f931e8365e15a089c68d6190000000000982051fd1e4ba744bbbe680e1fee14677ba1a3c3540bf7b1cdb606e857233e0e61bc6649ffff001d01e362990101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d0104ffffffff0100f2052a0100000043410496b538e853519c726a2c91e61ec11600ae1390813a627c66fb8be7947be63c52da7589379515d4e0a604f8141781e62294721166bf621e73a82cbf2342c858eeac00000000");
constexpr block_data block2_data = base16_array("010000004860eb18bf1b1620e37e9490fc8a427514416fd75159ab86688e9a8300000000d5fdcc541e25de1c7a5addedf24858b8bb665c9f36ef744ee42c316022c90f9bb0bc6649ffff001d08d2bd610101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d010bffffffff0100f2052a010000004341047211a824f55b505228e4c3d5194c1fcfaa15a456abdf37f9b9d97a4040afc073dee6c89064984f03385237d92167c13e236446b417ab79a0fcae412ae3316b77ac00000000");
constexpr block_data block3_data = base16_array("01000000bddd99ccfda39da1b108ce1a5d70038d0a967bacb68b6b63065f626a0000000044f672226090d85db9a9f2fbfe5f0f9609b387af7be5b7fbb7a1767c831c9e995dbe6649ffff001d05e0ed6d0101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d010effffffff0100f2052a0100000043410494b9d3e76c5b1629ecf97fff95d7a4bbdac87cc26099ada28066c6ff1eb9191223cd897194a08d0c2726c5747f1db49e8cf90e75dc3e3550ae9b30086f3cd5aaac00000000");
constexpr block_data block4_data = base16_array("010000004944469562ae1c2c74d9a535e00b6f3e40ffbad4f2fda3895501b582000000007a06ea98cd40ba2e3288262b28638cec5337c1456aaf5eedc8e9e5a20f062bdf8cc16649ffff001d2bfee0a90101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d011affffffff0100f2052a01000000434104184f32b212815c6e522e66686324030ff7e5bf08efb21f8b00614fb7690e19131dd31304c54f37baa40db231c918106bb9fd43373e37ae31a0befc6ecaefb867ac00000000");
constexpr block_data block5_data = base16_array("0100000085144a84488ea88d221c8bd6c059da090e88f8a2c99690ee55dbba4e00000000e11c48fecdd9e72510ca84f023370c9a38bf91ac5cae88019bee94d24528526344c36649ffff001d1d03e4770101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d0120ffffffff0100f2052a0100000043410456579536d150fbce94ee62b47db2ca43af0a730a0467ba55c79e2a7ec9ce4ad297e35cdbb8e42a4643a60eef7c9abee2f5822f86b1da242d9c2301c431facfd8ac00000000");
constexpr block_data block6_data = base16_array("01000000fc33f596f822a0a1951ffdbf2a897b095636ad871707bf5d3162729b00000000379dfb96a5ea8c81700ea4ac6b97ae9a9312b2d4301a29580e924ee6761a2520adc46649ffff001d189c4c970101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d0123ffffffff0100f2052a0100000043410408ce279174b34c077c7b2043e3f3d45a588b85ef4ca466740f848ead7fb498f0a795c982552fdfa41616a7c0333a269d62108588e260fd5a48ac8e4dbf49e2bcac00000000");
constexpr block_data block7_data = base16_array("010000008d778fdc15a2d3fb76b7122a3b5582bea4f21f5a0c693537e7a03130000000003f674005103b42f984169c7d008370967e91920a6a5d64fd51282f75bc73a68af1c66649ffff001d39a59c860101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d012bffffffff0100f2052a01000000434104a59e64c774923d003fae7491b2a7f75d6b7aa3f35606a8ff1cf06cd3317d16a41aa16928b1df1f631f31f28c7da35d4edad3603adb2338c4d4dd268f31530555ac00000000");
constexpr block_data block8_data = base16_array("010000004494c8cf4154bdcc0720cd4a59d9c9b285e4b146d45f061d2b6c967100000000e3855ed886605b6d4a99d5fa2ef2e9b0b164e63df3c4136bebf2d0dac0f1f7a667c86649ffff001d1c4b56660101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d012cffffffff0100f2052a01000000434104cc8d85f5e7933cb18f13b97d165e1189c1fb3e9c98b0dd5446b2a1989883ff9e740a8a75da99cc59a21016caf7a7afd3e4e9e7952983e18d1ff70529d62e0ba1ac00000000");
constexpr block_data block9_data = base16_array("01000000c60ddef1b7618ca2348a46e868afc26e3efc68226c78aa47f8488c4000000000c997a5e56e104102fa209c6a852dd90660a20b2d9c352423edce25857fcd37047fca6649ffff001d28404f530101000000010000000000000000000000000000000000000000000000000000000000000000ffffffff0704ffff001d0134ffffffff0100f2052a0100000043410411db93e1dcdb8a016b49840f8c53bc1eb68a382e97b1482ecad7b148a6909a5cb2e0eaddfb84ccf9744464f82e160bfa9b8b64f9d4c03f999b8643f656b412a3ac00000000");

using namespace chain;
const block genesis = system::settings{ chain::selection::mainnet }.genesis_block;
const block block1{ block1_data, true };
const block block2{ block2_data, true };
const block block3{ block3_data, true };
const block block4{ block4_data, true };
const block block5{ block5_data, true };
const block block6{ block6_data, true };
const block block7{ block7_data, true };
const block block8{ block8_data, true };
const block block9{ block9_data, true };


const block mock_block10
{
    header
    {
        0x31323334,
        block9_hash,
        hash_digest{ 0x10, 0xbb },
        0x41424344,
        0x51525354,
        0x61626364
    },
    transactions
    {
        transaction
        {
            0x01,
            inputs
            {
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x02
                },
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x03
                }
            },
            outputs
            {
                output
                {
                    0x04,
                    script::to_pay_key_hash_pattern({ 0x01 })
                }
            },
            0x05
        },
        transaction
        {
            0x06,
            inputs
            {
                input
                {
                    point{ block1.transactions_ptr()->front()->hash(false), 0x00 },
                    script{},
                    witness{},
                    0x07
                },
                input
                {
                    point{ block2.transactions_ptr()->front()->hash(false), 0x00 },
                    script{},
                    witness{},
                    0x08
                }
            },
            outputs
            {
                output
                {
                    0x09,
                    // "1BaMPFdqMUQ46BV8iRcwbVfsam57oBLMM"
                    script::to_pay_key_hash_pattern({ 0x02 })
                },
                output
                {
                    0x10,
                    // "31xsx7sPoS2UfoUAKfoXLX6wTPvpetyo7s"
                    script::to_pay_script_hash_pattern({ 0x03 })
                }
            },
            0x0a
        },
        transaction
        {
            0x0b,
            inputs
            {
                // Null points in non-first tx (coinbase confusion).
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x0c
                },
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x0d
                }
            },
            outputs
            {
                output
                {
                    0x0e,
                    script::to_pay_script_hash_pattern({ 0x04 })
                }
            },
            0x0f
        }
    }
};
const block mock_block11
{
    header
    {
        0x31323334,
        mock_block10.hash(),
        hash_digest{ 0x11, 0xbb },
        0x41424344,
        0x51525354,
        0x61626364
    },
    transactions
    {
        transaction
        {
            0x07,
            inputs
            {
                input
                {
                    point{ block3.transactions_ptr()->front()->hash(false), 0x00 },
                    script{},
                    witness{},
                    0x08
                },
                input
                {
                    point{ block4.transactions_ptr()->front()->hash(false), 0x00 },
                    script{},
                    witness{},
                    0x09
                }
            },
            outputs
            {
                output
                {
                    0x10,
                    // "1BaMPFdqMUQ46BV8iRcwbVfsam57oBLMM"
                    script::to_pay_key_hash_pattern({ 0x02 })
                },
                output
                {
                    0x11,
                    // "1BaMPFdqMUQ46BV8iRcwbVfsam57oBLMM"
                    script::to_pay_key_hash_pattern({ 0x02 })
                },
                output
                {
                    0x12,
                    // "31xsx7sPoS2UfoUAKfoXLX6wTPvpetyo7s"
                    script::to_pay_script_hash_pattern({ 0x03 })
                },
                output
                {
                    0x13,
                    // "31xsx7sPoS2UfoUAKfoXLX6wTPvpetyo7s"
                    script::to_pay_script_hash_pattern({ 0x03 })
                },
                output
                {
                    0x14,
                    // "31xsx7sPoS2UfoUAKfoXLX6wTPvpetyo7s"
                    script::to_pay_script_hash_pattern({ 0x03 })
                }
            },
            0x0a
        }
    }
};
const block mock_block12
{
    header
    {
        0x31323334,
        mock_block11.hash(),
        hash_digest{ 0x12, 0xbb },
        0x41424344,
        0x51525354,
        0x61626364
    },
    transactions
    {
        transaction
        {
            0x07,
            inputs
            {
                input
                {
                    point{ mock_block11.transactions_ptr()->front()->hash(false), 0x03 },
                    script{},
                    witness{},
                    0x08
                },
                input
                {
                    point{ mock_block11.transactions_ptr()->front()->hash(false), 0x04 },
                    script{},
                    witness{},
                    0x09
                }
            },
            outputs
            {
                output
                {
                    0x10,
                    // "1BaMPFdqMUQ46BV8iRcwbVfsam57oBLMM"
                    script::to_pay_key_hash_pattern({ 0x02 })
                }
            },
            0x0a
        }
    }
};
const block mock_block_a
{
    header
    {
        0x31323334,
        null_hash,
        hash_digest{ 0xbb },
        0x41424344,
        0x51525354,
        0x61626364
    },
    transactions
    {
        transaction
        {
            0x01,
            inputs
            {
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x02
                },
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x03
                }
            },
            outputs
            {
                output
                {
                    0x04,
                    script{}
                }
            },
            0x05
        },
        transaction
        {
            0x06,
            inputs
            {
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x07
                },
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x08
                }
            },
            outputs
            {
                output
                {
                    0x09,
                    script{}
                }
            },
            0x0a
        },
        transaction
        {
            0x0b,
            inputs
            {
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x0c
                },
                input
                {
                    point{},
                    script{},
                    witness{},
                    0x0d
                }
            },
            outputs
            {
                output
                {
                    0x0e,
                    script{}
                }
            },
            0x0f
        }
    }
};
const block block1a
{
    header
    {
        0x31323334,         // version
        block0_hash,        // previous_block_hash
        hash_digest{ 0x1a },// merkle_root
        0x41424344,         // timestamp
        0x51525354,         // bits
        0x61626364          // nonce
    },
    transactions
    {
        // This first transaction is *not* a coinbase.
        transaction         // tx#1
        {
            0x2a,           // version
            inputs
            {
                input
                {
                    point{ one_hash, 0x18 },            // missing prevout
                    script{ { { opcode::op_return }, { opcode::pick } } },
                    witness{ "[242424]" },
                    0x2a    // sequence
                },
                input
                {
                    point{ one_hash, 0x2a },            // missing prevout
                    script{ { { opcode::op_return }, { opcode::roll } } },
                    witness{ "[313131]" },
                    0x18    // sequence
                },
                input
                {
                    point{ hash_digest{ 0x02 }, 0x2b }, // missing prevout
                    script{ { { opcode::op_return }, { opcode::roll } } },
                    witness{ "[424242]" },
                    0x19    // sequence
                }
            },
            outputs
            {
                output
                {
                    0x18,   // value
                    script{ { { opcode::pick } } }
                },
                output
                {
                    0x2a,   // value
                    script{ { { opcode::roll } } }
                }
            },
            0x18            // locktime
        }
    }
};
const block block2a
{
    header
    {
        0x31323334,         // version
        block1a.hash(),     // previous_block_hash
        hash_digest{ 0x2a },// merkle_root
        0x41424344,         // timestamp
        0x51525354,         // bits
        0x61626364          // nonce
    },
    transactions
    {
        // This first transaction is *not* a coinbase.
        transaction         // tx#2
        {
            0xa2,           // version
            inputs
            {
                input
                {
                    // existing prevout
                    point{ block1a.transactions_ptr()->front()->hash(false), 0x00 },
                    script{ { { opcode::checkmultisig }, { opcode::pick } } },
                    witness{ "[242424]" },
                    0xa2    // sequence
                },
                input
                {
                    // existing prevout
                    point{ block1a.transactions_ptr()->front()->hash(false), 0x01 },
                    script{ { { opcode::checkmultisig }, { opcode::roll } } },
                    witness{ "[313131]" },
                    0x81    // sequence
                }
            },
            outputs
            {
                output
                {
                    0x81,   // value
                    script{ { { opcode::pick } } }
                }
            },
            0x81            // locktime
        },
        transaction         // tx#3
        {
            0xa2,           // version
            inputs
            {
                input
                {
                    point{ one_hash, 0x20 },    // missing prevout
                    script{ { { opcode::checkmultisig }, { opcode::pick } } },
                    witness{ "[242424]" },
                    0xa2    // sequence
                },
                input
                {
                    point{ one_hash, 0x21 },    // missing prevout
                    script{ { { opcode::checkmultisig }, { opcode::roll } } },
                    witness{ "[313131]" },
                    0x81    // sequence
                }
            },
            outputs
            {
                output
                {
                    0x81,   // value
                    script{ { { opcode::pick } } }
                }
            },
            0x81            // locktime
        }
    }
};
const block block3a
{
    header
    {
        0x31323334,         // version
        block2a.hash(),     // previous_block_hash
        hash_digest{ 0x3a },// merkle_root
        0x41424344,         // timestamp
        0x51525354,         // bits
        0x61626364          // nonce
    },
    transactions
    {
        // This first transaction is *not* a coinbase.
        transaction
        {
            0xa3,           // version
            inputs
            {
                input
                {
                    // existing prevout
                    point{ block1a.transactions_ptr()->front()->hash(false), 0x01 },
                    script{ { { opcode::checkmultisig }, { opcode::size } } },
                    witness{ "[949494]" },
                    0xa3    // sequence
                },
                input
                {
                    // existing prevout
                    point{ block1a.transactions_ptr()->front()->hash(false), 0x00 },
                    script{ { { opcode::checkmultisig }, { opcode::size } } },
                    witness{ "[919191]" },
                    0x83    // sequence
                }
            },
            outputs
            {
                output
                {
                    0x83,   // value
                    script{ { { opcode::pick } } }
                }
            },
            0x83            // locktime
        }
    }
};
const transaction tx4
{
    0xa5,           // version
    inputs
    {
        input
        {
            point{ block1a.transactions_ptr()->front()->hash(false), 0x00 },
            script{ { { opcode::checkmultisig }, { opcode::pick } } },
            witness{ "[252525]" },
            0xa5    // sequence
        },
        input
        {
            point{ block1a.transactions_ptr()->front()->hash(false), 0x01 },
            script{ { { opcode::checkmultisig }, { opcode::roll } } },
            witness{ "[353535]" },
            0x85    // sequence
        }
    },
    outputs
    {
        output
        {
            0x08,   // value
            script{ { { opcode::pick } } }
        }
    },
    0x85            // locktime
};
const transaction tx5
{
    0xa5,           // version
    inputs
    {
        input
        {
            point{ block1a.transactions_ptr()->front()->hash(false), 0x00 },
            script{ { { opcode::checkmultisig }, { opcode::pick } } },
            witness{ "[252525]" },
            0xa5    // sequence
        }
    },
    outputs
    {
        output
        {
            0x85,   // value
            script{ { { opcode::pick } } }
        }
    },
    0x85            // locktime
};
const block block1b
{
    header
    {
        0x31323334,         // version
        block0_hash,        // previous_block_hash
        hash_digest{ 0x1b },// merkle_root
        0x41424344,         // timestamp
        0x51525354,         // bits
        0x61626364          // nonce
    },
    transactions
    {
        // This first transaction is a coinbase.
        transaction     // tx#1
        {
            0xb1,
            inputs
            {
                input
                {
                    point{},
                    script{ { { opcode::checkmultisig }, { opcode::size } } },
                    witness{},
                    0xb1
                }
            },
            outputs
            {
                output
                {
                    0xb1,
                    script{ { { opcode::pick } } }
                },
                output
                {
                    0xb1,
                    script{ { { opcode::pick } } }
                }
            },
            0xb1
        }
    }
};
const block block2b
{
    header
    {
        0x31323334,         // version
        block1b.hash(),     // previous_block_hash
        hash_digest{ 0x2b },// merkle_root
        0x41424344,         // timestamp
        0x51525354,         // bits
        0x61626364          // nonce
    },
    transactions
    {
        // This first transaction is a coinbase.
        transaction     // tx#2
        {
            0xb2,
            inputs
            {
                input
                {
                    point{ block1b.transactions_ptr()->front()->hash(false), 0x00 },
                    script{ { { opcode::checkmultisig }, { opcode::size } } },
                    witness{},
                    0xb2
                },
                input
                {
                    point{ block1b.transactions_ptr()->front()->hash(false), 0x01 },
                    script{ { { opcode::checkmultisig }, { opcode::size } } },
                    witness{},
                    0xb2
                }
            },
            outputs
            {
                output
                {
                    0xb2,
                    script{ { { opcode::pick } } }
                }
            },
            0xb2
        }
    }
};
const transaction tx2b
{
    transaction
    {
        0xb1,
        inputs
        {
            input
            {
                // Spends block1b coinbase.
                point{ block1b.transactions_ptr()->front()->hash(false), 0x00 },
                script{ { { opcode::checkmultisig }, { opcode::size } } },
                witness{},
                0xb1
            }
        },
        outputs
        {
            output
            {
                0xb1,
                script{ { { opcode::pick } } }
            }
        },
        0xb1
    }
};

struct mock_block_value
{
    static constexpr hash_digest root = base16_hash("0f3842895590aae2727ca1b35c755d1d9e577a8f2640d972f9989bdcddc9c1e5");
    static constexpr uint32_t block_version{ 0 };
    static constexpr hash_digest previous_block_hash{ 1 };
    static constexpr hash_digest merkle_root{ root };
    static constexpr uint32_t timestamp{ 3 };
    static constexpr uint32_t bits{ 4 };
    static constexpr uint32_t nonce{ 5 };
    static constexpr uint32_t tx_version{ 6 };
    static constexpr uint32_t locktime{ 7 };
    static constexpr uint32_t sequence{ 8 };
    static constexpr uint64_t value{ 9 };
    static constexpr bool minimal{ true };
};

const block mock_block
{
    header // ptr<header>
    {
        mock_block_value::block_version,
        mock_block_value::previous_block_hash,
        mock_block_value::merkle_root,
        mock_block_value::timestamp,
        mock_block_value::bits,
        mock_block_value::nonce
    },
    transactions // ptr<vector<tx>>
    {
        // coinbase
        transaction  // first tx
        {
            mock_block_value::tx_version,
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
                                mock_block_value::minimal
                            },
                            operation
                            {
                                data_chunk{ 0xfe , 0xed }, // ptr<data_chunk>
                                mock_block_value::minimal
                            }
                        }
                    },
                    witness
                    {
                    },
                    mock_block_value::sequence
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
                    mock_block_value::sequence
                }
            },
            outputs
            {
                output // at least one output
                {
                    mock_block_value::value,  // no more than reward
                    script{}
                },
                output // at least one output
                {
                    mock_block_value::value,  // no more than reward
                    script{}
                }
            },
            mock_block_value::locktime
        }
    }
};

} // namespace test
