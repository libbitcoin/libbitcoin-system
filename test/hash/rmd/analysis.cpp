/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include "../../test.hpp"

// This can be integrated into round() using a [bool Trace = false] template
// argument. Dumping each round to a normal form provides a common view between
// the two common implementation approaches (buffer vs. circular queue), as
// described in FIPS180. This applies to both SHA and MD algorithm families.
// This may be useful when implementing vectorization and sha-ni/neon, as all
// reference/example code is implemented using the latter approach.
////if constexpr (Trace)
////{
////    if (!std::is_constant_evaluated())
////    {
////        std::cout <<
////              "0x" << encode_base16(to_big_endian(a)) <<
////            ", 0x" << encode_base16(to_big_endian(b)) <<
////            ", 0x" << encode_base16(to_big_endian(c)) <<
////            ", 0x" << encode_base16(to_big_endian(d)) <<
////            ", 0x" << encode_base16(to_big_endian(e)) <<
////            ", 0x" << encode_base16(to_big_endian(x)) <<
////            ", 0x" << encode_base16(to_big_endian(s)) <<
////            ", 0x" << encode_base16(to_big_endian(k)) <<
////            std::endl;
////    }
////}
