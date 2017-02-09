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
#include <bitcoin/bitcoin.hpp>
using namespace bc;

void test()
{
    big_number bignum;
    bignum.set_compact(0x1b0404cb);
    BITCOIN_ASSERT((bignum.hash() == hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));

    big_number bignum_max;
    bignum_max.set_compact(0x1d00ffff);
    BITCOIN_ASSERT((bignum_max.hash() == hash_digest{0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
    BITCOIN_ASSERT(bignum_max.compact() == 0x1d00ffff);

    BITCOIN_ASSERT(bignum < bignum_max);

    big_number other;
    other.set_hash(bignum.hash());
    BITCOIN_ASSERT(other.data() == bignum.data());

    BITCOIN_ASSERT(bignum_max <= libbitcoin::max_target());
    BITCOIN_ASSERT(bignum < libbitcoin::max_target());

    big_number arith, oper;
    arith.set_uint64(0x1f1f1f1f1f1f);
    oper.set_uint64(0x1f1f1f1f1f1f);
    arith *= oper;
    //BITCOIN_ASSERT((arith.data() == data_chunk{0x03, 0xc8, 0x8d, 0x52, 0x16, 0xdb, 0x98, 0xd4, 0x0f, 0x4a, 0x85, 0xc1}));

    big_number a, b;
    a.set_data({0x70});
    b.set_data({0x0c});
    BITCOIN_ASSERT(big_number(a - b) == 100);
    BITCOIN_ASSERT((big_number(a - b).data() == data_chunk{100}));
}

int main()
{
    test();
    return 0;
}

