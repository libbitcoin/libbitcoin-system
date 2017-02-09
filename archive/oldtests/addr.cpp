/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/address.hpp>
#include <bitcoin/utility/assert.hpp>
using namespace libbitcoin;

int main()
{
    payment_address addr;
    set_public_key(addr, {
        0x04, 0xa3, 0x9b, 0x9e, 0x4f, 0xbd, 0x21, 0x3e,
        0xf2, 0x4b, 0xb9, 0xbe, 0x69, 0xde, 0x4a, 0x11,
        0x8d, 0xd0, 0x64, 0x40, 0x82, 0xe4, 0x7c, 0x01,
        0xfd, 0x91, 0x59, 0xd3, 0x86, 0x37, 0xb8, 0x3f,
        0xbc, 0xdc, 0x11, 0x5a, 0x5d, 0x6e, 0x97, 0x05,
        0x86, 0xa0, 0x12, 0xd1, 0xcf, 0xe3, 0xe3, 0xa8,
        0xb1, 0xa3, 0xd0, 0x4e, 0x76, 0x3b, 0xdc, 0x5a,
        0x07, 0x1c, 0x0e, 0x82, 0x7c, 0x0b, 0xd8, 0x34,
        0xa5});
    BITCOIN_ASSERT(addr.encoded() == "1VayNert3x1KzbpzMGt2qdqrAThiRovi8");

    payment_address addr1;
    set_public_key_hash(addr1, short_hash{
        0x3a, 0x77, 0x5c, 0x1d, 0x2e, 0xb6, 0x12, 0x3b,
        0xc7, 0x74, 0xcf, 0xcb, 0xee, 0xb9, 0xdd, 0xc3,
        0x01, 0x79, 0x41, 0x6e});
    BITCOIN_ASSERT(addr1.encoded() == "16L9BPjFJ1zbVzpoxGrNrkQea6xL9optR7");

    payment_address addr2;
    addr2.set_encoded("16L9BPjFJ1zbVzpoxGrNrkQea6xL9optR7");
    BITCOIN_ASSERT((addr2.hash() == short_hash{
        0x3a, 0x77, 0x5c, 0x1d, 0x2e, 0xb6, 0x12, 0x3b,
        0xc7, 0x74, 0xcf, 0xcb, 0xee, 0xb9, 0xdd, 0xc3,
        0x01, 0x79, 0x41, 0x6e}));
    BITCOIN_ASSERT(addr2.type() == payment_type::pubkey_hash);
    return 0;
}

