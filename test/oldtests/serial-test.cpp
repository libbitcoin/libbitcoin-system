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
using namespace libbitcoin;

int main()
{
    data_chunk rawdat(decode_hex("46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000"));
    BITCOIN_ASSERT((rawdat == data_chunk{0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7, 0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1, 0xbb, 0x72, 0x44, 0x5b, 0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0, 0xc8, 0xdb, 0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00}));
    auto deserial = make_deserializer(rawdat.begin(), rawdat.end());
    output_point outpoint;
    outpoint.hash = deserial.read_hash();
    outpoint.index = deserial.read_4_bytes();
    log_debug() << encode_hex(outpoint.hash) << " : " << outpoint.index;
    BITCOIN_ASSERT((outpoint.hash == hash_digest{0x8e, 0xd5, 0xa0, 0xaf, 0x15, 0x1c, 0xdb, 0xc8, 0xc0, 0xc5, 0x46, 0xcd, 0xe2, 0x93, 0x34, 0xf1, 0x5b, 0x44, 0x72, 0xbb, 0xa1, 0x05, 0x39, 0x4a, 0x12, 0x21, 0xa7, 0xf0, 0x88, 0x24, 0x68, 0x46}));
    BITCOIN_ASSERT(outpoint.index == 0);
    data_chunk buff(36);
    auto serial = make_serializer(buff.begin());
    serial.write_hash(outpoint.hash);
    serial.write_4_bytes(outpoint.index);
    BITCOIN_ASSERT(buff == rawdat);
    return 0;
}

