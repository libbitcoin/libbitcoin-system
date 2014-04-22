///*
// * Copyright 2013 William Swanson
// *
// * This program is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License, version 2, as
// * published by the Free Software Foundation.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// */
//#include <bitcoin/bitcoin.hpp>
//
//#include <assert.h>
//#include <iostream>
//
//int main()
//{
//    using namespace bc;
//
//    // Integer round-tripping:
//    assert( 0x12345 == bigint( 0x12345).to_int());
//    assert(-0x12345 == bigint(-0x12345).to_int());
//
//    // Comparison operators:
//    bigint _1(1);
//    bigint _2000(2000);
//
//    assert(bigint(0x12345) < bigint(0x12346));
//    assert(bigint(-0x12346) < bigint(-0x12345));
//    assert(bigint(-1) < bigint(1));
//    assert(bigint(-2000) < bigint(-1));
//
//    assert(_1 < _2000);
//    assert(!(_2000 < _2000));
//    assert(!(_2000 < _1));
//
//    assert(_2000 > _1);
//    assert(!(_2000 > _2000));
//    assert(!(_1 > _2000));
//
//    assert(_1 <= _2000);
//    assert(_2000 <= _2000);
//    assert(!(_2000 <= _1));
//
//    assert(_2000 >= _1);
//    assert(_2000 >= _2000);
//    assert(!(_1 >= _2000));
//
//    assert(_2000 == _2000);
//    assert(!(_1 == _2000));
//
//    assert(_1 != _2000);
//    assert(!(_2000 != _2000));
//
//    // Zero:
//    assert( bigint(1));
//    assert(!bigint(0));
//
//    // Unary operators:
//    assert(bigint(42) == ~ ~bigint(42));
//    assert(bigint(42) == - -bigint(42));
//    assert(~bigint(0) == -bigint(1));
//    assert(~bigint(41) == -bigint(42));
//
//    // Bitwise logic:
//    assert(bigint(0x0008) == (bigint(0x0c) & bigint(0xff0a)));
//    assert(bigint(0xff0e) == (bigint(0x0c) | bigint(0xff0a)));
//    assert(bigint(0xff06) == (bigint(0x0c) ^ bigint(0xff0a)));
//
//    // ALU:
//    assert(bigint( 320) == bigint( 192) + bigint(128));
//    assert(bigint( -64) == bigint(-192) + bigint(128));
//    assert(bigint(  64) == bigint( 192) - bigint(128));
//    assert(bigint(-320) == bigint(-192) - bigint(128));
//
//    assert(bigint(0x10000) == bigint(0xff80) + bigint(0x80));
//    assert(bigint(0x10000) == bigint(0x80) + bigint(0xff80));
//
//    assert(bigint( 0xff00) == bigint(0xff80) - bigint(0x80));
//    assert(bigint(-0xff00) == bigint(0x80) - bigint(0xff80));
//
//    // Shifter:
//    assert(bigint( 0x00c0) == bigint(0x600) >> 3);
//    assert(bigint(-0x00c0) == bigint(-0x60000) >> 11);
//    assert(bigint( 0x0600) == bigint(0xc0) << 3);
//
//    // Multiplier:
//    assert(bigint( 500) == bigint( 50) * bigint( 10));
//    assert(bigint(-500) == bigint(-50) * bigint( 10));
//    assert(bigint(-500) == bigint( 50) * bigint(-10));
//    assert(bigint( 500) == bigint(-50) * bigint(-10));
//
//    assert(bigint(50000) == bigint(-5000) * bigint(-10));
//    assert(bigint(5000000) == bigint(-5000) * bigint(-1000));
//
//    assert(bigint( 1 << 14) == bigint( 0x80) * bigint( 0x80));
//    assert(bigint(-1 << 14) == bigint(-0x80) * bigint( 0x80));
//    assert(bigint(-1 << 14) == bigint( 0x80) * bigint(-0x80));
//
//    // Divider:
//    assert(bigint( 2) == bigint( 7) / bigint( 3));
//    assert(bigint(-3) == bigint(-7) / bigint( 3));
//    assert(bigint(-3) == bigint( 7) / bigint(-3));
//    assert(bigint( 2) == bigint(-7) / bigint(-3));
//    assert(bigint( 1) == bigint( 7) % bigint( 3));
//    assert(bigint( 2) == bigint(-7) % bigint( 3));
//    assert(bigint(-2) == bigint( 7) % bigint(-3));
//    assert(bigint(-1) == bigint(-7) % bigint(-3));
//
//    assert(bigint( 0) == bigint( 3) / bigint( 7));
//    assert(bigint(-1) == bigint(-3) / bigint( 7));
//    assert(bigint(-1) == bigint( 3) / bigint(-7));
//    assert(bigint( 0) == bigint(-3) / bigint(-7));
//    assert(bigint( 3) == bigint( 3) % bigint( 7));
//    assert(bigint( 4) == bigint(-3) % bigint( 7));
//    assert(bigint(-4) == bigint( 3) % bigint(-7));
//    assert(bigint(-3) == bigint(-3) % bigint(-7));
//
//    assert(bigint( 1000000) == bigint( 10000001) / bigint(10));
//    assert(bigint(-1000001) == bigint(-10000001) / bigint(10));
//    assert(bigint(1) == bigint( 10000001) % bigint(10));
//    assert(bigint(9) == bigint(-10000001) % bigint(10));
//
//    assert(bigint(0xfe) == bigint(0x7fff00) / bigint(0x80ff));
//    assert(bigint(0xfe) == bigint(0x7f80fe) / bigint(0x8001));
//    assert(bigint(0xff) == bigint(0x7f80ff) / bigint(0x8001));
//     // This was returning 0x00ffff before:
//    assert(bigint(-0xff) == bigint(0xff00) / bigint(-0x100));
//
//    // Large integers:
//    bigint bigr(bigint_from_dec("1000000000000"));
//    bigint big1(bigint_from_hex("140de851d4734634"));
//    bigint big2(bigint_from_hex("2bd93e1b0c3ce7f0d5e1fa8e"));
//    bigint bigp(bigint_from_hex("036f5aad28bd30e844cd1b5c0ee126566233b8d8")); // = big1 * big2
//
//    assert(bigp  ==  big1 * big2);
//    assert(-bigp == -big1 * big2);
//    assert(-bigp ==  big1 * -big2);
//    assert(bigp  == -big1 * -big2);
//
//    assert(big2  ==  bigp / big1);
//    assert(-big2 == -bigp / big1);
//    assert(-big2 ==  bigp / -big1);
//    assert(big2  == -bigp / -big1);
//
//    assert(big1  ==  bigp / big2);
//    assert(-big1 == -bigp / big2);
//    assert(-big1 ==  bigp / -big2);
//    assert(big1  == -bigp / -big2);
//
//    assert(bigr        ==  (bigp + bigr) % big1);
//    assert(big1 - bigr == -(bigp + bigr) % big1);
//    assert(bigr - big1 ==  (bigp + bigr) % -big1);
//    assert(-bigr       == -(bigp + bigr) % -big1);
//
//    assert(bigr        ==  (bigp + bigr) % big2);
//    assert(big2 - bigr == -(bigp + bigr) % big2);
//    assert(bigr - big2 ==  (bigp + bigr) % -big2);
//    assert(-bigr       == -(bigp + bigr) % -big2);
//
//    return 0;
//}
