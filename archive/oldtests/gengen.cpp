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
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/sha256.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/types.hpp>
#include <iostream>

void create_genesis_block()
{
    libbitcoin::data_chunk tx;
    tx.push_back(1);
    tx.push_back(0);
    tx.push_back(0);
    tx.push_back(0);
    // number of txin
    tx.push_back(1);
    // TXIN -----------------
    //   COutPoint -----------
    //     hash:
    libbitcoin::hash_digest prevhash{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (size_t i = 0; i < prevhash.size(); ++i)
        tx.push_back(0);
    auto prev_n = libbitcoin::to_little_endian<uint32_t>(UINT_MAX);
    libbitcoin::extend_data(tx, prev_n);

    const char* genesis_timestamped_message =
        "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    libbitcoin::log_info() << "Satoshi: " << genesis_timestamped_message;
    libbitcoin::data_chunk msg(genesis_timestamped_message, genesis_timestamped_message + strlen(genesis_timestamped_message));
    auto bits = libbitcoin::to_little_endian<uint32_t>(486604799);

    // TXIN scriptSig ---------------
    // length of script = 0x4d
    tx.push_back(0x4d);
    tx.push_back(4);
    libbitcoin::extend_data(tx, bits);
    tx.push_back(1);
    tx.push_back(4);
    tx.push_back(strlen(genesis_timestamped_message));
    libbitcoin::extend_data(tx, msg);

    libbitcoin::data_chunk txout;
    // nSequence
    libbitcoin::extend_data(tx, prev_n);

    // number of txout
    tx.push_back(1);
    // TXOUT ----------
    uint64_t value = 5000000000;
    auto coinage = libbitcoin::to_little_endian(value);
    libbitcoin::extend_data(tx, coinage);

    libbitcoin::data_chunk script_pubkey{0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27, 0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7, 0x10, 0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09, 0xa6, 0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6, 0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38, 0xc4, 0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12, 0xde, 0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57, 0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d, 0x5f};
    // push n bytes to stack...
    tx.push_back(0x43);
    //... and this is n
    tx.push_back(script_pubkey.size());
    libbitcoin::extend_data(tx, script_pubkey);
    // OP_CHECKSIG
    size_t OP_CHECKSIG = 172;
    tx.push_back(OP_CHECKSIG);

    // nLockTime
    tx.push_back(0);
    tx.push_back(0);
    tx.push_back(0);
    tx.push_back(0);

    /*for (auto i = tx.begin(); i != tx.end(); ++i)
    {
        unsigned char v = *i;
        std::cout << std::hex << (int)v << " ";
    }
    std::cout << "\n";
    */

    libbitcoin::serializer hasher;
    hasher.write_data(tx);
    libbitcoin::hash_digest hash = libbitcoin::generate_sha256_hash(hasher.get_data());
    /*
    for (auto i = hash.begin(); i != hash.end(); ++i)
        std::cout << std::hex << (int)*i << " ";
    std::cout << "\n";
    */

    libbitcoin::serializer block;
    block.write_4_bytes(1);
    // prev hash = 0
    for (size_t i = 0; i < libbitcoin::sha256_length; ++i)
        block.write_byte(0);
    block.write_data(libbitcoin::data_chunk(hash.rbegin(), hash.rend()));
    block.write_4_bytes(1231006505);
    block.write_4_bytes(0x1d00ffff);
    block.write_4_bytes(2083236893);

    hash = libbitcoin::generate_sha256_hash(block.get_data());
    for (auto i = hash.begin(); i != hash.end(); ++i)
        std::cout << std::hex << (int)*i << " ";
    std::cout << "\n";
}

int main()
{
    create_genesis_block();
    return 0;
}

