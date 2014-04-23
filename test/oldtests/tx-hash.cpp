/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin.hpp>
using namespace bc;

void hash_cmp_tx_b67d0729a14fcd45f88e53f7ec646abb47ed0a52dcdaeae28783a0038dae15c6()
{
    libbitcoin::script input_script;
    input_script.push_operation(libbitcoin::operation{libbitcoin::opcode::special, {0x30, 0x45, 0x02, 0x21, 0x00, 0x90, 0x60, 0x91, 0x43, 0xa8, 0xb7, 0x61, 0xe8, 0x5d, 0x16, 0x6e, 0x60, 0x13, 0x84, 0xc8, 0x52, 0x9a, 0x9e, 0x31, 0x35, 0xd3, 0x8a, 0xc4, 0x5f, 0xa2, 0xe3, 0xd4, 0xa3, 0x0b, 0x1a, 0x3a, 0x28, 0x02, 0x20, 0x78, 0xda, 0x18, 0x5b, 0x1c, 0x24, 0xd4, 0x59, 0x84, 0x69, 0xf5, 0x9e, 0x35, 0x46, 0x59, 0xff, 0x9d, 0xd8, 0xbf, 0x84, 0x69, 0x31, 0x9c, 0xc8, 0xe9, 0xf8, 0xa9, 0xaf, 0x03, 0xd9, 0xaa, 0x99, 0x01}});
    input_script.push_operation(libbitcoin::operation{libbitcoin::opcode::special, {0x04, 0x92, 0x12, 0xcb, 0x6f, 0xe6, 0xde, 0xde, 0xf0, 0x42, 0x13, 0xbe, 0x7f, 0xc4, 0x3b, 0x56, 0xe5, 0xd9, 0xf9, 0x72, 0x22, 0x29, 0xef, 0xbf, 0x20, 0x9b, 0x68, 0x97, 0x42, 0x43, 0x2d, 0x17, 0x6c, 0xd5, 0x54, 0xfe, 0xc4, 0x0a, 0x70, 0x5f, 0xe3, 0x6d, 0x2e, 0x1a, 0xda, 0x8c, 0x8a, 0x05, 0x86, 0x68, 0xdd, 0x92, 0xf7, 0xad, 0xf2, 0x6f, 0x2d, 0xa0, 0x11, 0x05, 0x77, 0xec, 0xe0, 0x3e, 0xdf}});

    libbitcoin::message::transaction_input tx_input{
        {{0x39, 0x7f, 0x72, 0x33, 0x34, 0xe2, 0x85, 0x9f, 0x00, 0x87, 0xb5, 0x64, 0xd8, 0xfb, 0xac, 0x8b, 0x2e, 0x22, 0xa2, 0xdb, 0xfc, 0x60, 0xc2, 0xdf, 0x35, 0x8d, 0x8e, 0xb4, 0xf9, 0xbe, 0xb3, 0x97}, 0},
        input_script,
        4294967295
    };

    libbitcoin::script output_script_1;
    output_script_1.push_operation(libbitcoin::operation{libbitcoin::opcode::dup, {}});
    output_script_1.push_operation(libbitcoin::operation{libbitcoin::opcode::hash160, {}});
    output_script_1.push_operation(libbitcoin::operation{libbitcoin::opcode::special, {0xff, 0xf4, 0x11, 0x06, 0xed, 0xf7, 0x1a, 0x83, 0x33, 0x37, 0x04, 0x1e, 0x08, 0x51, 0x22, 0x4e, 0xaa, 0x23, 0x67, 0x3a}});
    output_script_1.push_operation(libbitcoin::operation{libbitcoin::opcode::equalverify, {}});
    output_script_1.push_operation(libbitcoin::operation{libbitcoin::opcode::checksig, {}});

    libbitcoin::message::transaction_output tx_output_1;
    tx_output_1.value = 2188570650000;
    tx_output_1.output_script = output_script_1;

    libbitcoin::script output_script_2;
    output_script_2.push_operation(libbitcoin::operation{libbitcoin::opcode::dup, {}});
    output_script_2.push_operation(libbitcoin::operation{libbitcoin::opcode::hash160, {}});
    output_script_2.push_operation(libbitcoin::operation{libbitcoin::opcode::special, {0x54, 0xeb, 0x97, 0xc9, 0xe3, 0x43, 0xd8, 0xf2, 0x6e, 0x2c, 0xbf, 0x0c, 0xe8, 0x57, 0x79, 0xd1, 0x14, 0xc0, 0x63, 0x01}});
    output_script_2.push_operation(libbitcoin::operation{libbitcoin::opcode::equalverify, {}});
    output_script_2.push_operation(libbitcoin::operation{libbitcoin::opcode::checksig, {}});

    libbitcoin::message::transaction_output tx_output_2;
    tx_output_2.value = 4223000000;
    tx_output_2.output_script = output_script_2;

    libbitcoin::message::transaction tx;
    tx.version = 1;
    tx.locktime = 0;
    tx.inputs.push_back(tx_input);
    tx.outputs.push_back(tx_output_1);
    tx.outputs.push_back(tx_output_2);

    libbitcoin::hash_digest tx_hash = libbitcoin::hash_transaction(tx);
    BITCOIN_ASSERT((tx_hash == libbitcoin::hash_digest{0xb6, 0x7d, 0x07, 0x29, 0xa1, 0x4f, 0xcd, 0x45, 0xf8, 0x8e, 0x53, 0xf7, 0xec, 0x64, 0x6a, 0xbb, 0x47, 0xed, 0x0a, 0x52, 0xdc, 0xda, 0xea, 0xe2, 0x87, 0x83, 0xa0, 0x03, 0x8d, 0xae, 0x15, 0xc6}));
}

using namespace libbitcoin;

// 87abda4755e492de6149affbfc67d42a367f76c166c6bc31c8dfb916f74f66bb out 1
// a17b21f52859ed326d1395d8a56d5c7389f5fc83c17b9140a71d7cb86fdf0f5f inp 0
void test_testnet_tx()
{
    data_chunk txadat = decode_hex("0100000001845ad165bdc0f9b5829cf5a594c4148dfd89e24756303f3a8dabeb597afa589b010000008b483045022063c233df8efa3d1885e069e375a8eabf16b23475ef21bdc9628a513ee4caceb702210090a102c7b602043e72b34a154d495ac19b3b9e42acb962c399451f2baead8f4c014104b38f79037ad25b84a564eaf53ede93dec70b35216e6682aa71a47cefa2996ec49acfbb0a8730577c62ef9a7cc20c740aaaaee75419bef9640a4216c2b49c42d3ffffffff02000c022900000000434104c08c0a71ccbe838403e3870aa1ab871b0ab3a6014b0ba41f6df2b9aefea73134ecaa0b27797620e402a33799e9047f86519d9e43bbd504cf753c293752933f4fac406f40010000000062537a7652a269537a829178a91480677c5392220db736455533477d0bc2fba65502879b69537a829178a91402d7aa2e76d9066fb2b3c41ff8839a5c81bdca19879b69537a829178a91410039ce4fdb5d4ee56148fe3935b9bfbbe4ecc89879b6953ae00000000");
    data_chunk txbdat = decode_hex("0100000001bb664ff716b9dfc831bcc666c1767f362ad467fcfbaf4961de92e45547daab8701000000fd190100493046022100d73f633f114e0e0b324d87d38d34f22966a03b072803afa99c9408201f6d6dc6022100900e85be52ad2278d24e7edbb7269367f5f2d6f1bd338d017ca460008776614401473044022071fef8ac0aa6318817dbd242bf51fb5b75be312aa31ecb44a0afe7b49fcf840302204c223179a383bb6fcb80312ac66e473345065f7d9136f9662d867acf96c12a42015241048c006ff0d2cfde86455086af5a25b88c2b81858aab67f6a3132c885a2cb9ec38e700576fd46c7d72d7d22555eee3a14e2876c643cd70b1b0a77fbf46e62331ac4104b68ef7d8f24d45e1771101e269c0aacf8d3ed7ebe12b65521712bba768ef53e1e84fff3afbee360acea0d1f461c013557f71d426ac17a293c5eebf06e468253e00ffffffff0280969800000000001976a9140817482d2e97e4be877efe59f4bae108564549f188ac7015a7000000000062537a7652a269537a829178a91480677c5392220db736455533477d0bc2fba65502879b69537a829178a91402d7aa2e76d9066fb2b3c41ff8839a5c81bdca19879b69537a829178a91410039ce4fdb5d4ee56148fe3935b9bfbbe4ecc89879b6953ae00000000");
    satoshi_exporter ex;
    auto txa = ex.load_transaction(txadat), txb = ex.load_transaction(txbdat);
    BITCOIN_ASSERT(hash_transaction(txa) == decode_hex_digest<hash_digest>("87abda4755e492de6149affbfc67d42a367f76c166c6bc31c8dfb916f74f66bb"));
    BITCOIN_ASSERT(hash_transaction(txb) == decode_hex_digest<hash_digest>("a17b21f52859ed326d1395d8a56d5c7389f5fc83c17b9140a71d7cb86fdf0f5f"));
    script& prevout = txa.outputs[1].output_script;
    script& ins = txb.inputs[0].input_script;
    BITCOIN_ASSERT(prevout.run(ins, txb, 0, false));
}

int main()
{
    test_testnet_tx();
    return 0;
    hash_cmp_tx_b67d0729a14fcd45f88e53f7ec646abb47ed0a52dcdaeae28783a0038dae15c6();

    message::block genesis = genesis_block();
    BITCOIN_ASSERT((hash_block_header(genesis) ==
        hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 
                    0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 
                    0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 
                    0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f}));
    return 0;
}

